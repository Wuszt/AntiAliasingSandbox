#include <d3d11.h>
#include <algorithm>
#include "RenderTargetViewsManager.h"
#include "Window.h"
#include <cassert>
#include "ShadersManager.h"
#include "PostProcessor.h"
#include "DebugLog.h"
#include "Core.h"

using namespace std;

RenderTargetViewsManager::RenderTargetViewsManager(Window* const& window)
{
    s_instance = this;
    m_window = window;

    InitializeViewport(SizeType::Window);
    InitializeViewport(SizeType::Resolution);

    s_instance->SetViewport(SizeType::Window);

    window->AddResizeListener(OnWindowResize);
    window->AddResolutionChangeListener(OnResolutionChange);
}

RenderTargetViewsManager::~RenderTargetViewsManager()
{
}

void RenderTargetViewsManager::OnWindowResize(const int& width, const int& height)
{
    s_instance->InitializeViewport(SizeType::Window);
    s_instance->ResizeRTVs(SizeType::Window);

    s_instance->SetViewport(s_instance->m_currentViewportSizeType);
}

void RenderTargetViewsManager::OnResolutionChange(const int& width, const int& height)
{
    s_instance->InitializeViewport(SizeType::Resolution);
    s_instance->ResizeRTVs(SizeType::Resolution);

    s_instance->SetViewport(s_instance->m_currentViewportSizeType);
}

RenderTargetViewsManager* RenderTargetViewsManager::s_instance;

void RenderTargetViewsManager::CreateRTVComponents(ID3D11Texture2D*& tex, ID3D11RenderTargetView*& rtv, ID3D11ShaderResourceView*& srv, const SizeType& sizeType, int textureSamplesAmount, DXGI_FORMAT format)
{
    D3D11_TEXTURE2D_DESC textureDesc;
    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;

    ZeroMemory(&textureDesc, sizeof(textureDesc));

    if (sizeType == SizeType::Window)
    {
        textureDesc.Width = m_window->GetWidth();
        textureDesc.Height = m_window->GetHeight();
    }
    else
    {
        textureDesc.Width = m_window->GetResolutionWidth();
        textureDesc.Height = m_window->GetResolutionHeight();
    }

    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = format;
    textureDesc.SampleDesc.Count = textureSamplesAmount;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    Core::GetD3Device()->CreateTexture2D(&textureDesc, nullptr, &tex);

    D3D11_SHADER_RESOURCE_VIEW_DESC shDesc;
    shDesc.Format = format;
    shDesc.ViewDimension = textureSamplesAmount > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
    shDesc.Texture2D.MostDetailedMip = 0;
    shDesc.Texture2D.MipLevels = 1;

    Core::GetD3Device()->CreateShaderResourceView(tex, &shDesc, &srv);

    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = textureSamplesAmount > 1 ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

    Core::GetD3Device()->CreateRenderTargetView(tex, &renderTargetViewDesc, &rtv);
}

RTV* RenderTargetViewsManager::GetOrCreatePooledRTV(const SizeType& sizeType)
{
    if (!m_availableRTVs[sizeType].empty())
    {
        auto it = m_availableRTVs[sizeType].begin();
        RTV* result = *it;
        m_availableRTVs[sizeType].erase(it);
        return result;
    }

    return CreateNewRTV(sizeType, 1, DXGI_FORMAT_B8G8R8A8_UNORM);
}

RTV* RenderTargetViewsManager::CreateNewRTV(const SizeType& sizeType, int textureSamplesAmount, DXGI_FORMAT format)
{
    ID3D11Texture2D* tex;
    ID3D11RenderTargetView* rtv;
    ID3D11ShaderResourceView* srv;
    CreateRTVComponents(tex, rtv, srv, sizeType, textureSamplesAmount, format);

    return new RTV(rtv, tex, srv, textureSamplesAmount, format);
}

void RenderTargetViewsManager::ResizeRTVs(const SizeType& sizeType)
{
    std::unordered_set<RTV*> availableRTVs = m_availableRTVs[sizeType];

    SetViewport(sizeType);

    for (RTV* const& rtv : availableRTVs)
    {
        ID3D11Texture2D* tex;
        ID3D11RenderTargetView* d3rtv;
        ID3D11ShaderResourceView* srv;
        CreateRTVComponents(tex, d3rtv, srv, sizeType, rtv->m_samplesAmount, rtv->m_format);

        rtv->Reinitialize(d3rtv, tex, srv);
    }

    for (auto& it : m_acquiredRTVs)
    {
        if (it.second != sizeType)
            continue;

        ID3D11Texture2D* tex;
        ID3D11RenderTargetView* d3rtv;
        ID3D11ShaderResourceView* srv;
        CreateRTVComponents(tex, d3rtv, srv, sizeType, it.first->m_samplesAmount, it.first->m_format);

        PostProcessor::DrawPass("CopyingPP.fx", { it.first}, d3rtv);

        it.first->Reinitialize(d3rtv, tex, srv);
    }
}

void RenderTargetViewsManager::InitializeViewport(SizeType size)
{
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    if (size == SizeType::Window)
    {
        viewport.Width = (float)m_window->GetWidth();
        viewport.Height = (float)m_window->GetHeight();
    }
    else //if(size == SizeType::Resolution)
    {
        viewport.Width = (float)m_window->GetResolutionWidth();
        viewport.Height = (float)m_window->GetResolutionHeight();
    }

    m_viewPorts[size] = viewport;
}

RTV* RenderTargetViewsManager::AcquireRTV(const SizeType& sizeType, const int textureSamplesAmount, DXGI_FORMAT format)
{
    RTV* result = nullptr;
    
    if (textureSamplesAmount == 1 && format == DXGI_FORMAT_B8G8R8A8_UNORM)
        result = GetOrCreatePooledRTV(sizeType);
    else
        result = CreateNewRTV(sizeType, textureSamplesAmount, format);
    
    m_acquiredRTVs.emplace(result, sizeType);

    return result;
}

void RenderTargetViewsManager::ReleaseRTV(RTV* const& rtv)
{
    SizeType sizeType = m_acquiredRTVs[rtv];
    m_acquiredRTVs.erase(rtv);

    if (rtv->IsNotPooled())
    {
        rtv->Release();
        delete rtv;
        return;
    }

    m_availableRTVs[sizeType].insert(rtv);
}

void RenderTargetViewsManager::SetViewport(SizeType size)
{
    m_currentViewportSizeType = size;

    Core::GetD3DeviceContext()->RSSetViewports(1, &m_viewPorts[size]);
}

RTV::RTV(ID3D11RenderTargetView* const& rtv, ID3D11Texture2D* const& tex, ID3D11ShaderResourceView* const& srv, int samplesAmount, DXGI_FORMAT format) : m_samplesAmount(samplesAmount), m_format(format)
{
    Reinitialize(rtv, tex, srv);
}

RTV::~RTV()
{
    Release();
}

ID3D11RenderTargetView* RTV::GetRTV() const
{
    return m_rtv;
}

ID3D11Texture2D* RTV::GetTexture() const
{
    return m_texture;
}

ID3D11ShaderResourceView* RTV::GetSRV() const
{
    return m_srv;
}

void RTV::Reinitialize(ID3D11RenderTargetView* const& rtv, ID3D11Texture2D* const& tex, ID3D11ShaderResourceView* const& srv)
{
    Release();

    m_rtv = rtv;
    m_texture = tex;
    m_srv = srv;
}

void RTV::Release()
{
    if (m_rtv)
        m_rtv->Release();

    if (m_texture)
        m_texture->Release();

    if (m_srv)
        m_srv->Release();

    m_rtv = nullptr;
    m_texture = nullptr;
    m_srv = nullptr;
}

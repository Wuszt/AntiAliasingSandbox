#include <d3d11.h>
#include <algorithm>
#include "RenderTargetViewsManager.h"
#include "Window.h"
#include <cassert>
#include "ShadersManager.h"
#include "PostProcessor.h"
#include "DebugLog.h"

using namespace std;

RenderTargetViewsManager::RenderTargetViewsManager(ID3D11Device* const& device, ID3D11DeviceContext* const& context, Window* const& window)
{
    s_instance = this;
    m_d3Device = device;
    m_window = window;
    m_d3Context = context;

    window->AddResizeListener(OnWindowResize);
}

RenderTargetViewsManager::~RenderTargetViewsManager()
{
}
void RenderTargetViewsManager::OnWindowResize(const int& width, const int& height)
{
    s_instance->ResizeRTVs();
}

RenderTargetViewsManager* RenderTargetViewsManager::s_instance;

void RenderTargetViewsManager::CreateRTVComponents(ID3D11Texture2D*& tex, ID3D11RenderTargetView*& rtv)
{

    D3D11_TEXTURE2D_DESC textureDesc;
    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;

    ZeroMemory(&textureDesc, sizeof(textureDesc));

    textureDesc.Width = m_window->GetWidth();
    textureDesc.Height = m_window->GetHeight();
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    m_d3Device->CreateTexture2D(&textureDesc, nullptr, &tex);

    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

    m_d3Device->CreateRenderTargetView(tex, &renderTargetViewDesc, &rtv);
}

RTV* RenderTargetViewsManager::GetOrCreateRTV()
{
    if (!m_availableRTVs.empty())
    {
        auto it = m_availableRTVs.begin();
        RTV* result = *it;
        m_availableRTVs.erase(it);
        return result;
    }

    ID3D11Texture2D* tex;
    ID3D11RenderTargetView* rtv;
    CreateRTVComponents(tex, rtv);

    return new RTV(rtv, tex);
}

void RenderTargetViewsManager::ResizeRTVs()
{
    for (RTV* const& rtv : m_availableRTVs)
    {
        ID3D11Texture2D* tex;
        ID3D11RenderTargetView* d3rtv;
        CreateRTVComponents(tex, d3rtv);

        rtv->Reinitialize(d3rtv, tex);
    }

    for (RTV* const& rtv : m_acquiredRTVs)
    {
        ID3D11Texture2D* tex;
        ID3D11RenderTargetView* d3rtv;
        CreateRTVComponents(tex, d3rtv);

        PostProcessor::DrawPass(m_d3Device, m_d3Context, "CopyingPP.fx", { rtv->GetTexture() }, d3rtv);

        rtv->Reinitialize(d3rtv, tex);
    }
}

RTV* RenderTargetViewsManager::AcquireRTV()
{
    RTV* result = GetOrCreateRTV();
    m_acquiredRTVs.insert(result);

    return result;
}

void RenderTargetViewsManager::ReleaseRTV(RTV* const& rtv)
{
    m_availableRTVs.insert(rtv);
    m_acquiredRTVs.erase(rtv);
}

RTV::RTV(ID3D11RenderTargetView* const& rtv, ID3D11Texture2D* const& tex)
{
    Reinitialize(rtv, tex);
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

void RTV::Reinitialize(ID3D11RenderTargetView* const& rtv, ID3D11Texture2D* const& tex)
{
    Release();

    m_rtv = rtv;
    m_texture = tex;
}

void RTV::Release()
{
    if (m_rtv)
        m_rtv->Release();

    if (m_texture)
        m_texture->Release();

    m_rtv = nullptr;
    m_texture = nullptr;
}

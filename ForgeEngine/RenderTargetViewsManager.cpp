#include <d3d11.h>
#include <algorithm>
#include "RenderTargetViewsManager.h"
#include "Window.h"
#include <cassert>

using namespace std;

RenderTargetViewsManager::RenderTargetViewsManager(ID3D11Device* const& device, ID3D11DeviceContext* const& context, Window* const& window)
{
    s_instance = this;
    m_d3Device = device;
    m_window = window;
    m_d3Context = context;
}

RenderTargetViewsManager::~RenderTargetViewsManager()
{
}
void RenderTargetViewsManager::OnWindowResize(const int& width, const int& height)
{

}

RenderTargetViewsManager* RenderTargetViewsManager::s_instance;

RTV* RenderTargetViewsManager::CreateRTV()
{
    ID3D11Texture2D* tex;
    ID3D11RenderTargetView* rtv;

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

    return new RTV(rtv, tex);
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

    return CreateRTV();
}

RTV* RenderTargetViewsManager::AcquireRTV()
{
    RTV* result = GetOrCreateRTV();
    m_acquiredRTVs.insert(result);

    static float bgColor[4] = { (0.0f, 0.0f, 0.0f, 0.0f) };
    m_d3Context->ClearRenderTargetView(result->GetRTV(), bgColor);

    return result;
}

void RenderTargetViewsManager::ReleaseRTV(RTV* const& rtv)
{
    m_availableRTVs.insert(rtv);
    m_acquiredRTVs.erase(rtv);
}

RTV::RTV(ID3D11RenderTargetView* const& rtv, ID3D11Texture2D* const& tex)
{
    Initialize(rtv, tex);
}

ID3D11RenderTargetView* RTV::GetRTV()
{
    return m_rtv;
}

ID3D11Texture2D* RTV::GetTexture()
{
    return m_texture;
}

void RTV::Initialize(ID3D11RenderTargetView* const& rtv, ID3D11Texture2D* const& tex)
{
    m_rtv = rtv;
    m_texture = tex;
}

#pragma once

#include "UIRenderingSystem.h"
#include <FW1FontWrapper/FW1FontWrapper.h>
#include <DirectXMath.h>
#include <string>
#include "Core.h"
#include "RenderTargetViewsManager.h"
#include <DirectXTex/DirectXTex.h>
#include "Window.h"

using namespace DirectX;
using namespace std;

UIRenderingSystem::UIRenderingSystem()
{
    FW1CreateFactory(FW1_VERSION, &m_textFactory);
    m_textFactory->CreateFontWrapper(Core::GetD3Device(), L"Arial", &m_fontWrapper);
    m_rtv = Core::GetRTVsManager()->AcquireRTV(SizeType::Window);
}


UIRenderingSystem::~UIRenderingSystem()
{
    m_fontWrapper->Release();
    m_textFactory->Release();

    Core::GetRTVsManager()->ReleaseRTV(m_rtv);
}

void UIRenderingSystem::OnBeforeDrawing()
{
    ID3D11RenderTargetView* rtv = m_rtv->GetRTV();

    float bgColor[4] = { (0.0f, 0.0f, 0.0f, 0.0f) };
    Core::GetD3DeviceContext()->ClearRenderTargetView(rtv, bgColor);
}

void UIRenderingSystem::Print(const std::string& text, const float& size, const float& x, const float& y, const DirectX::XMFLOAT4& color, const TextAnchor& anchor /*= TextAnchor::Center*/) const
{
    Core::GetRTVsManager()->SetViewport(SizeType::Window);

    ID3D11RenderTargetView* rtv = m_rtv->GetRTV();
    Core::GetD3DeviceContext()->OMSetRenderTargets(1, &rtv, nullptr);

    UINT clr = (UINT)(min(max(color.x, 0.0f), 1.0f) * 255)
        | ((UINT)(min(max(color.y, 0.0f), 1.0f) * 255)) << 8
        | ((UINT)(min(max(color.z, 0.0f), 1.0f) * 255)) << 16
        | ((UINT)(min(max(color.w, 0.0f), 1.0f) * 255)) << 24;

    UINT flags = FW1_RESTORESTATE;
    flags |= ((UINT)anchor & (UINT)TextAnchor::Bottom) ? FW1_BOTTOM : 0;
    flags |= ((UINT)anchor & (UINT)TextAnchor::Left) ? FW1_LEFT : 0;
    flags |= ((UINT)anchor & (UINT)TextAnchor::Right) ? FW1_RIGHT : 0;

    if (flags & FW1_LEFT || flags & FW1_RIGHT)
        flags |= ((UINT)anchor & (UINT)TextAnchor::Center) ? FW1_VCENTER : 0;
    else
        flags |= ((UINT)anchor & (UINT)TextAnchor::Center) ? FW1_CENTER : 0;

    m_fontWrapper->DrawString(Core::GetD3DeviceContext(), wstring(text.begin(), text.end()).c_str(), size, x, y, clr, flags);
}

RTV* UIRenderingSystem::GetRTV() const
{
    return m_rtv;
}

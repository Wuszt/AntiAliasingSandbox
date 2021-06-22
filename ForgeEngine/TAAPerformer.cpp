#include "TAAPerformer.h"
#include "AAHelpers.h"
#include <DirectXMath.h>
#include "Core.h"
#include "Window.h"
#include "PostProcessor.h"
#include "RenderTargetViewsManager.h"
#include "Camera.h"
#include "DebugLog.h"
#include <d3d11.h>
#include <DirectXCommonClasses/InputClass.h>

using namespace DirectX;

TAAPerformer::TAAPerformer(std::function<void(RTV*)> func) : IAAPerformer(func)
{
    D3D11_BUFFER_DESC cbbd;
    ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

    cbbd.Usage = D3D11_USAGE_DEFAULT;
    cbbd.ByteWidth = sizeof(cbTAA);
    cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    Core::GetD3Device()->CreateBuffer(&cbbd, nullptr, &m_cbTAABuff);
}

void TAAPerformer::OnEnable()
{
    IAAPerformer::OnEnable();

    m_prevRTV = Core::GetRTVsManager()->AcquireRTV(SizeType::Resolution);
}

void TAAPerformer::OnDisable()
{
    IAAPerformer::OnDisable();

    Core::GetRTVsManager()->ReleaseRTV(m_prevRTV);
}

void TAAPerformer::DrawScene()
{
    static int framesCount = 1;
    ++framesCount;

    const int samplesAmount = 4;

    XMFLOAT2* offsets = new XMFLOAT2[samplesAmount];

    Get2x2RGSS(offsets);

    XMFLOAT2 jitter = offsets[(framesCount) % samplesAmount];
    XMFLOAT2 prevJitter = offsets[(framesCount - 1) % samplesAmount];

    m_cbTAA.JitterOffset = jitter;
    m_cbTAA.JitterOffset.x -= prevJitter.x;
    m_cbTAA.JitterOffset.y -= prevJitter.y;
    m_cbTAA.JitterOffset.x *= 0.25f;
    m_cbTAA.JitterOffset.y *= 0.25f;

    Core::GetD3DeviceContext()->UpdateSubresource(m_cbTAABuff, 0, nullptr, &m_cbTAA, 0, 0);
    Core::GetD3DeviceContext()->PSSetConstantBuffers(static_cast<UINT>(PixelCBIndex::TAA), 1, &m_cbTAABuff);

    Core::GetCamera()->SetOffset({ jitter.x / Core::GetWindow()->GetResolutionWidth(), -jitter.y / Core::GetWindow()->GetResolutionHeight() });
    m_drawFunc(m_output);

    Core::GetCamera()->SetOffset({ 0.0f,0.0f });
}

void TAAPerformer::PostProcessing()
{
    RTV* tmp = Core::GetRTVsManager()->AcquireRTV(SizeType::Resolution);

    PostProcessor::DrawPass("TAA.fx", { m_output, m_prevRTV, Core::GetVelocityBuffer(), Core::GetDepthStencilBuffer() }, tmp);

    Core::GetRTVsManager()->ReleaseRTV(m_prevRTV);
    m_prevRTV = tmp;

    Core::GetD3DeviceContext()->CopyResource(m_output->GetTexture(), tmp->GetTexture());

}

std::string TAAPerformer::GetName()
{
    return "TAA";
}

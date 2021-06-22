#include "MSAAPerformer.h"
#include <d3d11.h>
#include "Core.h"
#include <DirectXCommonClasses/InputClass.h>
#include "PostProcessor.h"
#include "RenderTargetViewsManager.h"

MSAAPerformer::MSAAPerformer(std::function<void(RTV*)> drawFunc, std::function<void()> initializeDepthStencilBuffFunc) : IAAPerformer(drawFunc)
{
    D3D11_BUFFER_DESC cbbd;
    ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

    cbbd.Usage = D3D11_USAGE_DEFAULT;
    cbbd.ByteWidth = sizeof(cbMSAA);
    cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    Core::GetD3Device()->CreateBuffer(&cbbd, nullptr, &m_cbMSAABuff);

    m_initializeDepthStencilBuffFunc = initializeDepthStencilBuffFunc;
}

void MSAAPerformer::OnEnable()
{
    IAAPerformer::OnEnable();
    m_sampleAmount = 2;
    Refresh();
}

void MSAAPerformer::OnDisable()
{
    IAAPerformer::OnDisable();
    m_sampleAmount = 1;
    Refresh();

    Core::GetRTVsManager()->ReleaseRTV(m_temporaryRTV);
}

void MSAAPerformer::DrawScene()
{
    m_drawFunc(m_temporaryRTV);
}

void MSAAPerformer::PostProcessing()
{
    m_cbMSAA.SamplesAmount = m_sampleAmount;
    Core::GetD3DeviceContext()->UpdateSubresource(m_cbMSAABuff, 0, nullptr, &m_cbMSAA, 0, 0);
    Core::GetD3DeviceContext()->PSSetConstantBuffers(static_cast<UINT>(PixelCBIndex::MSAA), 1, &m_cbMSAABuff);

    if (m_standardResolve)
        Core::GetD3DeviceContext()->ResolveSubresource(m_output->GetTexture(), 1, m_temporaryRTV->GetTexture(), 1, DXGI_FORMAT_R8G8B8A8_UNORM);
    else
        PostProcessor::DrawPass("Resolve.hlsl", { m_temporaryRTV }, m_output);
}

void MSAAPerformer::FillDepthStencilDescWithDefaultValues(D3D11_TEXTURE2D_DESC& desc)
{
    desc.SampleDesc.Count = m_sampleAmount;
}

void MSAAPerformer::Update()
{
    IAAPerformer::Update();

    if (InputClass::GetKeyDown(DIK_Q))
    {
        m_sampleAmount *= 2;

        if (m_sampleAmount > 8)
            m_sampleAmount = 2;

        Refresh();
    }

    if (InputClass::GetKeyDown(DIK_R))
    {
        m_standardResolve = !m_standardResolve;
    }
}

std::string MSAAPerformer::GetName()
{
    return "MSAAx" + std::to_string(m_sampleAmount) + (m_standardResolve ? " Standard" : " Custom") + " Resolve";
}

void MSAAPerformer::Refresh()
{
    m_temporaryRTV = Core::GetRTVsManager()->AcquireRTV(SizeType::Resolution, m_sampleAmount);
    m_initializeDepthStencilBuffFunc();
}

void MSAAPerformer::SetVariant(int variantIndex)
{
    m_sampleAmount = (int)pow(2, (variantIndex + 2) / 2);
    m_standardResolve = variantIndex % 2 == 0;

    Refresh();
}

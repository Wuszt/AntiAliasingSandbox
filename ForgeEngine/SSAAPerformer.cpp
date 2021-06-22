#include "SSAAPerformer.h"
#include <d3d11.h>
#include "Core.h"
#include "PostProcessor.h"
#include <DirectXCommonClasses/InputClass.h>
#include "Camera.h"
#include "AAHelpers.h"
#include "Core.h"
#include "RenderTargetViewsManager.h"
#include "Window.h"

SSAAPerformer::SSAAPerformer(std::function<void(RTV*)> func) : IAAPerformer(func)
{
    D3D11_BUFFER_DESC cbbd;
    ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

    cbbd.Usage = D3D11_USAGE_DEFAULT;
    cbbd.ByteWidth = sizeof(cbSSAA);
    cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    Core::GetD3Device()->CreateBuffer(&cbbd, nullptr, &m_cbSSAABuff);
}

void SSAAPerformer::OnEnable()
{
    IAAPerformer::OnEnable();
    m_samplesAmount = 4;
}

void SSAAPerformer::DrawScene()
{
    XMFLOAT2* offsets = new XMFLOAT2[m_samplesAmount];

    if (m_samplesAmount == 4)
    {
        if (m_rotated)
            Get2x2RGSS(offsets);
        else
            Get2x2Grid(offsets);
    }
    else if (m_samplesAmount == 8)
        Get4x4Checker(offsets);
    else if (m_samplesAmount == 16)
        Get4x4Grid(offsets);
    else if (m_samplesAmount == 32)
        Get8x8Checker(offsets);
    else if (m_samplesAmount == 64)
        Get8x8Grid(offsets);

    DivideByResolution(offsets, m_samplesAmount, Core::GetWindow()->GetResolutionWidth(), Core::GetWindow()->GetResolutionHeight());

    for (int i = 0; i < m_samplesAmount; ++i)
    {
        m_rtvs.push_back(Core::GetRTVsManager()->AcquireRTV(SizeType::Resolution));
        Core::GetCamera()->SetOffset(offsets[i]);
        m_drawFunc(m_rtvs[i]);
    }

    Core::GetCamera()->SetOffset({ 0.0f,0.0f });

    delete[] offsets;
}

void SSAAPerformer::PostProcessing()
{
    m_cbSSAA.TexturesAmount = (int)m_rtvs.size();
    Core::GetD3DeviceContext()->UpdateSubresource(m_cbSSAABuff, 0, nullptr, &m_cbSSAA, 0, 0);
    Core::GetD3DeviceContext()->PSSetConstantBuffers(static_cast<UINT>(PixelCBIndex::SSAA), 1, &m_cbSSAABuff);

    PostProcessor::DrawPass("SuperSampling.fx", m_rtvs, m_output);

    for (int i = 0; i < m_samplesAmount; ++i)
    {
        Core::GetRTVsManager()->ReleaseRTV(m_rtvs[i]);
    }

    m_rtvs.clear();
}

void SSAAPerformer::Update()
{
    IAAPerformer::Update();

    if (InputClass::GetKeyDown(DIK_Q))
    {
        m_samplesAmount *= 2;

        if (m_samplesAmount > 64)
            m_samplesAmount = 4;
    }

    if (InputClass::GetKeyDown(DIK_R))
        m_rotated = !m_rotated;
}

std::string SSAAPerformer::GetName()
{
    return "SSAAx" + std::to_string(m_samplesAmount) + ((m_rotated && m_samplesAmount == 4) ? "R" : "");
}

void SSAAPerformer::SetVariant(int variantIndex)
{
    ++variantIndex;

    m_rotated = variantIndex == 1;

    if (variantIndex == 1)
    {
        variantIndex = 2;
    }

    m_samplesAmount = (int)pow(2,variantIndex);
}

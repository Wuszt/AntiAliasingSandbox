#include "LightsManager.h"
#include <DirectXMath.h>
#include "Core.h"
#include <d3d11.h>
#include <Windows.h>
#include <DirectXCommonClasses/Time.h>
#include <cassert>
#include "DirectionalLight.h"
#include <exception>

using namespace DirectX;

struct cbLights
{
    XMFLOAT3 Ambient;
    int DirectionalLightsAmount;
    cbDirectionalLight DirectionalLights[10];
};

LightsManager::LightsManager()
{
    D3D11_BUFFER_DESC cbbd;
    ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

    cbbd.Usage = D3D11_USAGE_DEFAULT;
    cbbd.ByteWidth = sizeof(cbLights);
    cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    assert(cbbd.ByteWidth % 16 == 0);

    Core::GetD3Device()->CreateBuffer(&cbbd, nullptr, &m_buffer);
}


LightsManager::~LightsManager()
{
}

void LightsManager::OnDrawingScene()
{
    cbLights lights;

    lights.Ambient = m_ambient;

    for (int i = 0; i < m_directionalLights.size(); ++i)
    {
        lights.DirectionalLights[i] = m_directionalLights[i]->GetData();
    }
    lights.DirectionalLightsAmount = (int)m_directionalLights.size();


    Core::GetD3DeviceContext()->UpdateSubresource(m_buffer, 0, nullptr, &lights, 0, 0);
    Core::GetD3DeviceContext()->VSSetConstantBuffers(static_cast<UINT>(VertexCBIndex::Light), 1, &m_buffer);
}

void LightsManager::AddLight(Light* const& light)
{
    DirectionalLight* directional = dynamic_cast<DirectionalLight*>(light);

    if (directional)
    {
        m_directionalLights.push_back(directional);
        return;
    }

    throw std::exception();
}

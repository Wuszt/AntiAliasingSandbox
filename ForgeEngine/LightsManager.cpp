#include "LightsManager.h"
#include <DirectXMath.h>
#include "Core.h"
#include <d3d11.h>
#include <Windows.h>
#include <DirectXCommonClasses/Time.h>
#include <cassert>

using namespace DirectX;

struct cbLights
{
    XMFLOAT3 Ambient;
    int DirectionalLightsAmount;
    DirectionalLight DirectionalLights[10];
};

LightsManager::LightsManager()
{
    D3D11_BUFFER_DESC cbbd;
    ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

    cbbd.Usage = D3D11_USAGE_DEFAULT;
    cbbd.ByteWidth = sizeof(cbLights);
    cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

  //  assert(cbbd.ByteWidth % 16 == 0);

    Core::GetD3Device()->CreateBuffer(&cbbd, nullptr, &m_buffer);
}


LightsManager::~LightsManager()
{
}

void LightsManager::OnDrawingScene()
{
    cbLights lights;
    lights.Ambient = XMFLOAT3(0.3f, 0.3f, 0.3f);
    lights.DirectionalLights[0].Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
    XMFLOAT3 dir = XMFLOAT3(2.0f * sin(Time::GetTime()), -1.0f, 0.0f);
    XMVECTOR tmp = XMLoadFloat3(&dir);
    tmp = XMVector3Normalize(tmp);
    XMStoreFloat3(&dir, tmp);
    lights.DirectionalLights[0].Direction = dir;
    lights.DirectionalLightsAmount = 1;

    Core::GetD3DeviceContext()->UpdateSubresource(m_buffer, 0, nullptr, &lights, 0, 0);
    Core::GetD3DeviceContext()->VSSetConstantBuffers(static_cast<UINT>(VertexCBIndex::Light), 1, &m_buffer);
}

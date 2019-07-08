#pragma once
#include <DirectXMath.h>

struct ID3D11Buffer;

struct DirectionalLight
{
    DirectX::XMFLOAT3 Direction;
    float pad0;
    DirectX::XMFLOAT3 Color;
    float pad1;
};


class LightsManager
{
public:
    LightsManager();
    ~LightsManager();

    void OnDrawingScene();

    ID3D11Buffer* m_buffer;
};


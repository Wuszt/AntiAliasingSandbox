#pragma once
#include <DirectXMath.h>

enum class VertexCBIndex
{
    PerFrame = 0,
    Light = 1,
    PerObject = 2,
    Material = 3
};

enum class PixelCBIndex
{
    PerFrame = 0,
};


struct cbPerFrame
{
    float Time;
    DirectX::XMFLOAT3 CameraPos;
};

struct cbPerObject
{
    DirectX::XMMATRIX W;
    DirectX::XMMATRIX WVP;
};

struct cbMaterial
{
    DirectX::XMFLOAT3 Diffuse;
    float Pad0;
    DirectX::XMFLOAT3 Specular;
    float Pad1;
};
#pragma once
#include <DirectXMath.h>

enum class VertexCBIndex
{
    PerFrame = 0,
    Light = 1,
    PerObject = 2,
};

enum class PixelCBIndex
{
    PerFrame = 0,
};


struct cbPerFrame
{
    float Time;
    DirectX::XMFLOAT3 Alignment;
};

struct cbPerObject
{
    DirectX::XMMATRIX W;
    DirectX::XMMATRIX WVP;
};
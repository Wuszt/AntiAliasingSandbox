#pragma once
#include <DirectXMath.h>

enum class VertexCBIndex
{
    None = -1,
    GlobalInfo = 0,
    PerFrame = 1,
    CameraInfo = 2,
    Light = 3,
    PerObject = 4,
    Material = 5,
    TAA = 6
};

enum class PixelCBIndex
{
    None = -1,
    GlobalInfo = 0,
    PerFrame = 1,
    CameraInfo = 2,
    SSAA = 3,
    MSAA = 4,
    TAA = 6,
};


struct cbPerFrame
{
    float Time;
    DirectX::XMFLOAT3 pad0;
};

struct cbCameraInfo
{
    DirectX::XMFLOAT3 CameraPos;
    float pad0;
    DirectX::XMFLOAT2 Jitter;
    DirectX::XMFLOAT2 pad1;
};

struct cbPerObject
{
    DirectX::XMMATRIX W;
    DirectX::XMMATRIX WVP;
    DirectX::XMMATRIX PrevWVP;
};

struct cbMaterial
{
    DirectX::XMFLOAT3 Diffuse;
    float Pad0;
    DirectX::XMFLOAT3 Specular;
    float Pad1;
};

struct cbGlobalInfo
{
    DirectX::XMFLOAT2 Resolution;
    DirectX::XMFLOAT2 TexelSize;
};

struct cbSSAA
{
    int TexturesAmount;
    float pad0;
    float pad1;
    float pad2;
};

struct cbMSAA
{
    int SamplesAmount;
    float pad0;
    float pad1;
    float pad2;
};

struct cbTAA
{
    DirectX::XMFLOAT2 JitterOffset;
    DirectX::XMFLOAT2 pad0;
};
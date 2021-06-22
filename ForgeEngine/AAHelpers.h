#pragma once

namespace DirectX
{
    struct XMFLOAT2;
}

void Get2x2Grid(DirectX::XMFLOAT2 offsets[4]);
void Get2x2RGSS(DirectX::XMFLOAT2 offsets[4]);

void Get4x4Checker(DirectX::XMFLOAT2 offsets[8]);
void Get4x4Grid(DirectX::XMFLOAT2 offsets[16]);

void Get8x8Checker(DirectX::XMFLOAT2 offsets[32]);
void Get8x8Grid(DirectX::XMFLOAT2 offsets[64]);

void DivideByResolution(DirectX::XMFLOAT2* offsets, int amount, int width, int height);

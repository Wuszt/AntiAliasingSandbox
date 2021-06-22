#include "AAHelpers.h"
#include <DirectXMath.h>
#include "DebugLog.h"

using namespace DirectX;

void Get2x2Grid(XMFLOAT2 offsets[4])
{
    float offset = 0.5f;

    offsets[0] = XMFLOAT2(offset, offset);
    offsets[1] = XMFLOAT2(-offset, -offset);
    offsets[2] = XMFLOAT2(-offset, offset);
    offsets[3] = XMFLOAT2(offset, -offset);
}

void Get2x2RGSS(DirectX::XMFLOAT2 offsets[4])
{
    offsets[0] = XMFLOAT2(0.25f, 0.75f);
    offsets[1] = XMFLOAT2(-0.75f, 0.25f);
    offsets[2] = XMFLOAT2(0.75f, -0.25f);
    offsets[3] = XMFLOAT2(-0.25f, -0.75f);
}

void Get4x4Grid(DirectX::XMFLOAT2 offsets[16])
{
    float offset = 0.75f;

    for (int i = 0; i < 16; ++i)
    {
        offsets[i].x = -offset;
        offsets[i].y = -offset;

        offsets[i].x += (i % 4) * 0.5f;
        offsets[i].y += (i / 4) * 0.5f;
    }
}

void Get4x4Checker(DirectX::XMFLOAT2 offsets[8])
{
    float offset = 0.75f;

    for (int i = 0; i < 16; i += 2)
    {
        offsets[i / 2].x = -offset;
        offsets[i / 2].y = -offset;

        offsets[i / 2].x += ((i % 4) + ((i / 4) % 2)) * 0.5f;
        offsets[i / 2].y += (i / 4) * 0.5f;
    }
}

void Get8x8Checker(DirectX::XMFLOAT2 offsets[32])
{
    float offset = 0.875f;

    for (int i = 0; i < 64; i+=2)
    {
        offsets[i / 2].x = -offset;
        offsets[i / 2].y = -offset;

        offsets[i / 2].x += ((i % 8) + ((i / 8) % 2))  * 0.25f;
        offsets[i / 2].y += (i / 8) * 0.25f;
    }
}

void Get8x8Grid(DirectX::XMFLOAT2 offsets[64])
{
    float offset = 0.875f;

    for (int i = 0; i < 64; ++i)
    {
        offsets[i].x = -offset;
        offsets[i].y = -offset;

        offsets[i].x += (i % 8) * 0.25f;
        offsets[i].y += (i / 8) * 0.25f;
    }
}

void DivideByResolution(DirectX::XMFLOAT2* offsets, int amount, int width, int height)
{
    for (int i = 0; i < amount; ++i)
    {
        offsets[i].x /= width;
        offsets[i].y /= height;
    }
}

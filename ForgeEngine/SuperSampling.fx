#include "CommonPP.fxh"

cbuffer cbSSAA : register(b3)
{
    int TexturesAmount;
};

Texture2D Textures[64];

static float2 offset = float2(1 / 1920.0f, 1 / 1080.0f);

float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 result = 0;

    for (int i = 0; i < TexturesAmount;++i)
    {
        result += Textures[i].Sample(PointSampler, input.Tex);
    }

    result /= TexturesAmount;

    result.a = 1.0f;

    return result;
}
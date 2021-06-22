#include "CommonPP.fxh"

Texture2D Tex0 : register(t0);
Texture2D<float2> Tex1 : register(t1);

static float2 offset = float2(1 / 1920.0f, 1 / 1080.0f);

float4 PS(VS_OUTPUT input) : SV_Target
{
    return Tex0.Sample(PointSampler, input.Tex) + 0.5f * float4(Tex1.Sample(PointSampler, input.Tex), 0.0f, 1.0f);
}
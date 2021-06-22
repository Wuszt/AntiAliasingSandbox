#include "CommonPP.fxh"

Texture2D Tex : register(t0);
Texture2D Outline : register(t1); //Tex with edges

static float2 offset = float2(1 / 1920.0f, 1 / 1080.0f);

float4 PS(VS_OUTPUT input) : SV_Target
{
    return Tex.Sample(PointSampler, input.Tex) * (1.0f - Outline.Sample(PointSampler, input.Tex).r);
}
#include "CommonPP.fxh"

Texture2D Tex0 : register(t0);
Texture2D Tex1 : register(t1);

float4 PS(VS_OUTPUT input) : SV_Target
{
    return Tex0.Sample(PointSampler, input.Tex) + Tex1.Sample(PointSampler, input.Tex);
}
#include "CommonPP.fxh"

Texture2D Texture;

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float4 clr = Texture.Sample(PointSampler, input.Tex);

    clr = clr * clr;

    clr.r = sqrt(clr.r * 0.299f + clr.g * 0.587f + clr.b * 0.114f);

    return clr.rrra;
}
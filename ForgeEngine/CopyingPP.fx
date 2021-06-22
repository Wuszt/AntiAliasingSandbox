#include "CommonPP.fxh"

Texture2D Texture;

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    return Texture.Sample(PointSampler, input.Tex);

}
#include "CommonPP.fxh"

Texture2D Texture;
SamplerState Sampler;

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    return Texture.Sample(Sampler, input.Tex);

}
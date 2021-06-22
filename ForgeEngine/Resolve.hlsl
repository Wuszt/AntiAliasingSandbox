#include "CommonPP.fxh"

cbuffer cbMSAA : register(b4)
{
    int SamplesAmount;
};

Texture2DMS<float4> tex : register(t0);

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float4 clr = 0;

    for (int i = 0; i < SamplesAmount;++i)
    {
        clr += tex.Load(uint2(input.Pos.xy), i);
    }

    return clr / SamplesAmount;
}
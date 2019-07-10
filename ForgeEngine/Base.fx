#include "Common.fxh"
#include "Light.fxh"

cbuffer cbPerObject : register(b2)
{
    float4x4 W;
    float4x4 WVP;
};

Texture2D ObjTexture;
SamplerState ObjSamplerState;

struct VS_INPUT
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD;
    float3 Diffuse : COLOR0;
    float3 Specular : COLOR1;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Pos = mul(float4(input.Pos, 1.0f), WVP);
    output.TexCoord = input.TexCoord;

    float3 worldNormal = normalize(mul(input.Normal, W).xyz);
    float3 worldPos = mul(float4(input.Pos, 1.0f), W).xyz;

    CalcLighting(worldPos, worldNormal, output.Diffuse, output.Specular); 

    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    return ObjTexture.Sample(ObjSamplerState, input.TexCoord) * float4(input.Diffuse, 1.0f) + float4(input.Specular, 0.0f);
}

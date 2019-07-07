#include "Common.fxh"

cbuffer cbPerObject : register(b2)
{
    float4x4 W;
    float4x4 WVP;
};

Texture2D ObjTexture;
SamplerState ObjSamplerState;

struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
    float4 VertexColor : COLOR0;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
    float4 VertexColor : COLOR;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Pos = mul(input.Pos, WVP);
    output.Normal = mul(input.Normal, W);
    output.Normal = normalize(output.Normal);
    output.TexCoord = input.TexCoord;
    output.VertexColor = input.VertexColor;

    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    return input.Normal;
}

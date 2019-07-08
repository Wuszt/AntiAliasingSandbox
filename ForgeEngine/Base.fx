#include "Common.fxh"

cbuffer cbPerObject : register(b2)
{
    float4x4 W;
    float4x4 WVP;
};

struct DirectionalLight
{
    float3 Direction;
    float3 Color;
};

cbuffer cbLights : register(b1)
{
    float3 Ambient;
    int DirectionalLightsAmount;
    DirectionalLight DirectionalLights[10];
}

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
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
    float4 Light : COLOR;
};

float4 CalcDirectionalLighting(float4 normal)
{
    float3 result = 0;

    for (int i = 0; i < DirectionalLightsAmount; ++i)
    {
        result += saturate(dot(normal, -DirectionalLights[i].Direction)) * DirectionalLights[i].Color;
    }

    return float4(result, 1.0f);
}

float4 CalcLighting(float4 pos, float4 normal)
{
    return float4(Ambient, 1.0f) + CalcDirectionalLighting(normal);
}

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Pos = mul(float4(input.Pos, 1.0f), WVP);
    output.TexCoord = input.TexCoord;
    output.Normal = input.Normal;
    output.Light = CalcLighting(output.Pos, float4(input.Normal, 0.0f));

    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    return ObjTexture.Sample(ObjSamplerState, input.TexCoord) * input.Light;
}

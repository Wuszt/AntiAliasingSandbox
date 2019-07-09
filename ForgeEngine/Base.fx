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
    float2 TexCoord : TEXCOORD;
    float4 Light : COLOR0;
    float4 Specular : COLOR1;
};

float Specular(float3 pos,float3 normal, float3 lightDir)
{
    float3 V = normalize(CameraPos - pos);
    float3 R = lightDir + 2 * dot(-normalize(lightDir), normalize(normal)) * normal;

    return saturate(dot(V, R));

}

float4 CalcDirectionalLighting(float3 pos, float3 normal)
{
    float3 result = 0;

    for (int i = 0; i < DirectionalLightsAmount; ++i)
    {
        result += saturate(dot(normal, -DirectionalLights[i].Direction)) * DirectionalLights[i].Color;
    }

    return float4(result, 1.0f);
}

float4 CalcLighting(float4 pos, float3 normal)
{
    return float4(Ambient, 1.0f) + CalcDirectionalLighting(pos, normal);
}

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Pos = mul(float4(input.Pos, 1.0f), WVP);
    output.TexCoord = input.TexCoord;

    float4 worldNormal = normalize(mul(input.Normal, W));
    float4 worldPos = mul(float4(input.Pos, 1.0f), W);

    output.Light = 0.5f * CalcLighting(worldPos, worldNormal); 
    output.Specular = Specular(worldPos, worldNormal, DirectionalLights[0].Direction);
    output.Specular *= output.Specular;

    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    return ObjTexture.Sample(ObjSamplerState, input.TexCoord) * input.Light + input.Specular;

}

#include "Common.fxh"
#include "Light.fxh"

cbuffer cbPerObject : register(b4)
{
    float4x4 W;
    float4x4 WVP;
    float4x4 PrevWVP;
};

cbuffer cbMaterial : register(b5)
{
    float3 Diffuse;
    float3 Specular;
}

cbuffer cbTAA : register(b6)
{
    float2 JitterOffset;
}

Texture2D ObjTexture;

struct VS_INPUT
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD1;
    float3 Diffuse : COLOR0;
    float3 Specular : COLOR1;
    float4 PrevPos : PREVPOS;
};

struct PSOutput
{
    float4 Color : SV_Target0;
    float2 Velocity : SV_Target1;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Pos = mul(float4(input.Pos, 1.0f), WVP);
    output.PrevPos = mul(float4(input.Pos, 1.0f), PrevWVP);
    output.TexCoord = input.TexCoord * 10.0f;

    float3 worldNormal = normalize(mul(input.Normal, W).xyz);
    float3 worldPos = mul(float4(input.Pos, 1.0f), W).xyz;

    CalcLighting(worldPos, worldNormal, CameraPos, output.Diffuse, output.Specular);

    output.Diffuse *= Diffuse;
    output.Specular *= Specular;
    
    return output;
}

PSOutput PS(VS_OUTPUT input) : SV_TARGET
{
    PSOutput output;
    output.Color = ObjTexture.Sample(LinearSampler, input.TexCoord) * float4(input.Diffuse, 1.0f) + float4(input.Specular, 0.0f);

    float2 prevPositionSS = (input.PrevPos.xy / input.PrevPos.w) * float2(0.5f, -0.5f) + 0.5f;
    prevPositionSS *= Resolution;
    output.Velocity = input.Pos.xy - prevPositionSS;
    output.Velocity -= JitterOffset;
    output.Velocity /= Resolution;
    
    if (length(output.Velocity) < 0.00001f)
        output.Velocity = 0.0f;

    return output;
}

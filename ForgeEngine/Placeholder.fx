
cbuffer cbPerObject
{
    float4x4 WVP;
};


struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
};

VS_OUTPUT VS(float4 inPos : POSITION, float2 inTexCoord : TEXCOORD)
{
    VS_OUTPUT output;

    output.Pos = mul(inPos, WVP);

    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    return float4(1.0f, 0.0f, 1.0f, 1.0f);
}

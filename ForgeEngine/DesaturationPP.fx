
Texture2D Texture;
SamplerState Sampler;

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
};

VS_OUTPUT VS(VS_OUTPUT input)
{
    return input;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float4 tmp = Texture.Sample(Sampler, input.Tex);

    tmp.r = (tmp.r * 0.3f + tmp.g * 0.59f + tmp.b * 0.11f);

    return tmp.rrra;
}
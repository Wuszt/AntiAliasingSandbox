
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
    return Texture.Sample(Sampler, input.Tex);

}
Texture2D Tex : register(t0);
SamplerState Sampler : register(s0);

static float2 offset = float2(1 / 1920.0f, 1 / 1080.0f);

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
};

VS_OUTPUT VS(VS_OUTPUT input)
{
    return input;
}

float4 MySimplify(float3 clr)
{
    float l = length(clr);

    clr /= l;
    if (l < 0.95f)
    {
        l = (int) (10.0f * l);
        //l -= l % 2;
        l /= 10.0f;
    }
    clr *= l;

    return float4(clr, 1.0f);
}


float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float3 clr = Tex.Sample(Sampler, input.Tex).rgb;
    return MySimplify(clr);
}
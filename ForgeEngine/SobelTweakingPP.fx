Texture2D Tex : register(t0);
SamplerState Sampler : register(s0);

static const float2 offset = float2(1 / 1920.0f, 1 / 1080.0f);
static const float PI = 3.14159265f;

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
};

VS_OUTPUT VS(VS_OUTPUT input)
{
    return input;
}

float2 ExtracTexandY(float4 input)
{
    return input.xy * 2.0f - 1.0f;
}

float3 GetDataForPixelWithOffset(float2 mainCoords, float2 off)
{
    float4 samp = Tex.Sample(Sampler, mainCoords + off * offset);
    return float3(ExtracTexandY(samp), samp.z);
}

float NonMaximumSuppression(VS_OUTPUT input)
{
    float3 data = GetDataForPixelWithOffset(input.Tex, float2(0, 0));

    float mg = data.z;

    float angle = atan2(data.y, data.x) * 180 / PI;

    return float4(mg, mg, mg, 1.0f);

    if ((angle >= 0.0f && 45.0f >= angle) || (angle >= -180.0f && -135.0f >= angle))
    {
        float top0 = GetDataForPixelWithOffset(input.Tex, float2(1, 0)).z;
        float top1 = GetDataForPixelWithOffset(input.Tex, float2(1, 1)).z;

        float bot0 = GetDataForPixelWithOffset(input.Tex, float2(-1, 0)).z;
        float bot1 = GetDataForPixelWithOffset(input.Tex, float2(-1, -1)).z;

        float x = abs(data.y / data.z);

        float interpTop = (top1 - top0) * x + top0;
        float interpBot = (bot1 - bot0) * x + bot0;

        if (interpTop > mg || interpBot > mg)
            mg = 0.0f;
    }
    else if ((angle >= 45.0f && 90.0f >= angle) || (angle >= -135.0f && -90.0f >= angle))
    {
        float top0 = GetDataForPixelWithOffset(input.Tex, float2(0, 1)).z;
        float top1 = GetDataForPixelWithOffset(input.Tex, float2(1, 1)).z;

        float bot0 = GetDataForPixelWithOffset(input.Tex, float2(0, -1)).z;
        float bot1 = GetDataForPixelWithOffset(input.Tex, float2(-1, -1)).z;

        float x = abs(data.x / data.z);

        float interpTop = (top1 - top0) * x + top0;
        float interpBot = (bot1 - bot0) * x + bot0;

        if (interpTop > mg || interpBot > mg)
            mg = 0.0f;
    }
    else if ((angle >= 90.0f && 135.0f >= angle) || (angle >= -90.0f && -45.0f >= angle))
    {
        float top0 = GetDataForPixelWithOffset(input.Tex, float2(0, 1)).z;
        float top1 = GetDataForPixelWithOffset(input.Tex, float2(-1, 1)).z;

        float bot0 = GetDataForPixelWithOffset(input.Tex, float2(0, -1)).z;
        float bot1 = GetDataForPixelWithOffset(input.Tex, float2(1, -1)).z;

        float x = abs(data.x / data.z);

        float interpTop = (top1 - top0) * x + top0;
        float interpBot = (bot1 - bot0) * x + bot0;

        if (interpTop > mg || interpBot > mg)
            mg = 0.0f;
    }
    else if ((angle >= 135.0f && 180.0f >= angle) || (angle >= -45.0f && 0.0f >= angle))
    {
        float top0 = GetDataForPixelWithOffset(input.Tex, float2(-1, 0)).z;
        float top1 = GetDataForPixelWithOffset(input.Tex, float2(-1, 1)).z;

        float bot0 = GetDataForPixelWithOffset(input.Tex, float2(1, 0)).z;
        float bot1 = GetDataForPixelWithOffset(input.Tex, float2(1, -1)).z;

        float x = abs(data.y / data.z);

        float interpTop = (top1 - top0) * x + top0;
        float interpBot = (bot1 - bot0) * x + bot0;

        if (interpTop > mg || interpBot > mg)
            mg = 0.0f;
    }


    return mg;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float e = NonMaximumSuppression(input);

    if (e > 0.45f)
        e = 1.0f;
    else if (e < 0.3f)
        e = 0.0f;

    return float4(e, e, e, 1.0f);
}
#include "CommonPP.fxh"

Texture2D Curr : register(t0);
Texture2D Prev : register(t1);
Texture2D<float2> Velocity : register(t2);
Texture2D Depth : register(t3);

float2 GetClosestTexelCoords(float2 tex)
{
    float min = 100.0f;
    float2 coords = 0;

    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float2 currentCoords = tex + float2(x, y) * TexelSize;
            float current = Depth.Sample(PointSampler, currentCoords).r;
            if (current < min)
            {
                min = current;
                coords = currentCoords;
            }
        }
    }

    return coords;
}

float4 GetClampedColor(float2 texCoords, float4 input, inout float4 clrMin, inout float4 clrMax)
{
    clrMin = 9999999.0f;
    clrMax = -9999999.0f;

    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float4 tmp = Curr.Sample(PointSampler, texCoords + float2(x, y) * TexelSize);
            clrMin = min(clrMin, tmp);
            clrMax = max(clrMax, tmp);
        }
    }

    return min(clrMax, max(clrMin, input));
}

float Luminance(in float3 clr)
{
    return dot(clr, float3(0.299f, 0.587f, 0.114f));
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float2 coords = GetClosestTexelCoords(input.Tex);
    float2 velocity = Velocity.Sample(PointSampler, coords);

    float4 curr = Curr.Sample(PointSampler, input.Tex);
    float4 prev = Prev.Sample(PointSampler, input.Tex - velocity);
    
    float4 clrMin;
    float4 clrMax;

    prev = GetClampedColor(input.Tex, prev, clrMin, clrMax);

    float a = 0.1f;
    float b = 0.9f;

    float4 clr = (a * curr + b * prev) / (a + b);

    return clr;
}
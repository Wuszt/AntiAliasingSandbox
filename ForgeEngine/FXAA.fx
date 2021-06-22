#include "CommonPP.fxh"

Texture2D Texture : register(t0);
Texture2D Desaturated : register(t1);

#define EDGE_THRESHOLD_MIN 0.0312f
#define EDGE_THRESHOLD_MAX 0.125f
#define SUBPIXEL_QUALITY 0.75f
#define ITERATIONS 100

struct LuminanceData
{
    float Center;
    float Up;
    float Down;
    float Left;
    float Right;
    float UpLeft;
    float UpRight;
    float DownLeft;
    float DownRight;

    float DownUp;
    float LeftRight;
    float LeftCorners;
    float RightCorners;
    float UpCorners;
    float DownCorners;

    float Min;
    float Max;
    float Range;
};

float GetLuma(float2 baseTexCoords, int texelOffsetX, int texelOffsetY)
{
    return Desaturated.Sample(LinearSampler, baseTexCoords + float2(texelOffsetX, texelOffsetY) * TexelSize).r;
}

LuminanceData GetLuminanceData(float2 currentUV)
{
    LuminanceData data;
    data.Center = GetLuma(currentUV, 0, 0);
    data.Up = GetLuma(currentUV, 0, 1);
    data.Down = GetLuma(currentUV, 0, -1);
    data.Left = GetLuma(currentUV, -1, 0);
    data.Right = GetLuma(currentUV, 1, 0);
    data.UpLeft = GetLuma(currentUV, -1, 1);
    data.UpRight = GetLuma(currentUV, 1, 1);
    data.DownLeft = GetLuma(currentUV, -1, -1);
    data.DownRight = GetLuma(currentUV, 1, -1);

    data.DownUp = data.Down + data.Up;
    data.LeftRight = data.Left + data.Right;
    data.LeftCorners = data.UpLeft + data.DownLeft;
    data.RightCorners = data.UpRight + data.DownRight;
    data.UpCorners = data.UpLeft + data.UpRight;
    data.DownCorners = data.DownLeft + data.DownRight;

    float horizontalMin = min(data.Left, data.Right);
    float verticalMin = min(data.Up, data.Down);

    float horizontalMax = max(data.Left, data.Right);
    float verticalMax = max(data.Up, data.Down);

    data.Min = min(horizontalMin, verticalMin);
    data.Max = max(horizontalMax, verticalMax);
    data.Range = data.Max - data.Min;

    return data;
}

bool IsTooDarkOrContrastIsToLow(LuminanceData data)
{
    return data.Range < max(EDGE_THRESHOLD_MIN, data.Max * EDGE_THRESHOLD_MAX);
}

bool IsHorizontal(LuminanceData data)
{
    float edgeHorizontal = abs(-2.0f * data.Left + data.LeftCorners) + abs(-2.0f * data.Center + data.DownUp) * 2.0f + abs(-2.0f * data.Right + data.RightCorners);
    float edgeVertical = abs(-2.0f * data.Up + data.UpCorners) + abs(-2.0f * data.Center + data.LeftRight) * 2.0f + abs(-2.0f * data.Down + data.DownCorners);

    return edgeHorizontal >= edgeVertical;
}


float4 PS(VS_OUTPUT input) : SV_TARGET
{
    LuminanceData data = GetLuminanceData(input.Tex);

    if (IsTooDarkOrContrastIsToLow(data))
        return Texture.Sample(LinearSampler, input.Tex);

    bool isHorizontal = IsHorizontal(data);

    float luma1 = isHorizontal ? data.Down : data.Left;
    float luma2 = isHorizontal ? data.Up : data.Right;

    float gradient1 = luma1 - data.Center;
    float gradient2 = luma2 - data.Center;

    bool is1Steepest = abs(gradient1) >= abs(gradient2);

    float gradientScaled = 0.25f * max(abs(gradient1), abs(gradient2));

    float stepLength = isHorizontal * TexelSize.y + !isHorizontal * TexelSize.x;
    stepLength -= is1Steepest * 2 * stepLength;

    float lumaLocalAverage = 0.5f * (is1Steepest * luma1 + !is1Steepest * luma2 + data.Center);

    float2 currentUv = input.Tex + stepLength * 0.5f * float2(!isHorizontal, isHorizontal);

    float2 offset = float2(isHorizontal * TexelSize.x, !isHorizontal * TexelSize.y);

    float2 uv1 = currentUv - offset;
    float2 uv2 = currentUv + offset;

    float lumaEnd1;
    float lumaEnd2;

    bool reached1 = false;
    bool reached2 = false;
    bool reachedBoth = false;

    if (!reachedBoth)
    {
        for (int i = 1; i < ITERATIONS; i++)
        {
            if (!reached1)
            {
                lumaEnd1 = Desaturated.Sample(LinearSampler, uv1).r;
                lumaEnd1 -= lumaLocalAverage;
            }

            if (!reached2)
            {
                lumaEnd2 = Desaturated.Sample(LinearSampler, uv2).r;
                lumaEnd2 -= lumaLocalAverage;
            }

            reached1 = abs(lumaEnd1) >= gradientScaled;
            reached2 = abs(lumaEnd2) >= gradientScaled;
            reachedBoth = reached1 && reached2;

            if (!reached1)
            {
                uv1 -= offset;
            }

            if (!reached2)
            {
                uv2 += offset;
            }

            if (reachedBoth)
            {
                break;
            }
        }
    }

    float distance1 = isHorizontal * (input.Tex.x - uv1.x) + !isHorizontal * (input.Tex.y - uv1.y);
    float distance2 = isHorizontal * (uv2.x - input.Tex.x) + !isHorizontal * (uv2.y - input.Tex.y);

    bool isDirection1 = distance1 < distance2;

    float distanceFinal = min(distance1, distance2);

    float edgeThickness = distance1 + distance2;

    float pixelOffset = -distanceFinal / edgeThickness + 0.5;

    bool isLumaCenterSmaller = data.Center < lumaLocalAverage;

    bool correctVariation = ((isDirection1 * lumaEnd1 + !isDirection1 * lumaEnd2) < 0.0) != isLumaCenterSmaller;

    pixelOffset *= correctVariation;

    float lumaAverage = (1.0f / 12.0f) * (2.0 * (data.DownUp + data.LeftRight) + data.LeftCorners + data.RightCorners);
    float subPixelOffset1 = clamp(abs(lumaAverage - data.Center) / data.Range, 0.0, 1.0);
    float subPixelOffset2 = (-2.0 * subPixelOffset1 + 3.0) * subPixelOffset1 * subPixelOffset1;
    float subPixelOffsetFinal = subPixelOffset2 * subPixelOffset2 * SUBPIXEL_QUALITY;

    pixelOffset = max(pixelOffset, subPixelOffsetFinal);

    float2 finalUv = input.Tex;
    if (isHorizontal)
    {
        finalUv.y += pixelOffset * stepLength;
    }
    else
    {
        finalUv.x += pixelOffset * stepLength;
    }
    
    return Texture.Sample(LinearSampler, finalUv);
}
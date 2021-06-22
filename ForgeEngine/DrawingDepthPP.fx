#include "CommonPP.fxh"

Texture2D Depth : register(t0);

float4 PS(VS_OUTPUT input) : SV_Target
{
    float depth = Depth.Sample(PointSampler, input.Tex);
    depth = depth * depth;
    depth = depth * depth;
    depth = depth * depth;
    depth = depth * depth;

    return depth;
}
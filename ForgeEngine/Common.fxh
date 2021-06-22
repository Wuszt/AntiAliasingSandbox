SamplerState PointSampler : register(s0);
SamplerState LinearSampler : register(s1);

//TODO: implement in cpp
cbuffer cbGlobalInfo : register(b0)
{
    float2 Resolution;
    float2 TexelSize;
};

cbuffer cbPerFrame : register(b1)
{
    float Time;
};

cbuffer cbCameraInfo : register(b2)
{
    float3 CameraPos;
    float2 Jitter;
};
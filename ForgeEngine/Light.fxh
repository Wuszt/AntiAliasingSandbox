struct DirectionalLight
{
    float3 Direction;
    float3 Color;
};

cbuffer cbLights : register(b1)
{
    float3 Ambient;
    int DirectionalLightsAmount;
    DirectionalLight DirectionalLights[10];
}

float CalcSpecular(float3 pos, float3 normal, float3 lightDir)
{
    float3 V = normalize(CameraPos - pos);
    float3 R = lightDir + 2 * dot(-normalize(lightDir), normalize(normal)) * normal;

    return saturate(dot(V, R));
}

void CalcDirectionalLighting(float3 pos, float3 normal, inout float3 diffuse, inout float3 specular)
{
    for (int i = 0; i < DirectionalLightsAmount; ++i)
    {
        diffuse += saturate(dot(normal, -DirectionalLights[i].Direction)) * DirectionalLights[i].Color;
        specular += CalcSpecular(pos, normal, DirectionalLights[i].Direction) * DirectionalLights[i].Color;
    }
}

void CalcLighting(float3 pos, float3 normal, out float3 diffuse, out float3 specular)
{
    diffuse = Ambient;

    CalcDirectionalLighting(pos, normal, diffuse, specular);

    specular *= specular;
    specular *= specular;
    //specular *= specular;
    //specular *= specular;
}
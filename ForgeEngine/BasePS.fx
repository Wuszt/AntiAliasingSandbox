Texture2D ObjTexture;
SamplerState ObjSamplerState;

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

float4 PS(PS_INPUT input) : SV_TARGET
{
    return ObjTexture.Sample(ObjSamplerState, input.TexCoord);

}

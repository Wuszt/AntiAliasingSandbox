#pragma once
#include <unordered_map>
#include <vector>

struct ID3D11ShaderResourceView;

enum class TextureTypes
{
    Unknown = -1,
    Diffuse,
    Specular,
};

struct Material
{
    std::unordered_map<TextureTypes, std::vector<ID3D11ShaderResourceView*>> SRVs;
};


#pragma once
#include <unordered_map>
#include <vector>

struct ID3D11ShaderResourceView;

struct Material
{
    std::vector<ID3D11ShaderResourceView*> SRVs;
};


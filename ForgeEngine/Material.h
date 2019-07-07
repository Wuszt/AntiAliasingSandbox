#pragma once
#include <unordered_map>
#include <vector>

struct ID3D11ShaderResourceView;
struct ID3D11InputLayout;
struct D3D11_INPUT_ELEMENT_DESC;
struct CachedShaders;

class Material
{
public:
    std::vector<ID3D11ShaderResourceView*> Textures;
    std::string ShaderPath;
    std::vector<D3D11_INPUT_ELEMENT_DESC> Layout;

    const CachedShaders* GetShaders() const;
    ID3D11InputLayout* GetInputLayout();

private:
    uint64_t m_shaderLastModTime;
    ID3D11InputLayout* m_inputLayout;
};


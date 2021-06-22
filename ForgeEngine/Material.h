#pragma once
#include <unordered_map>
#include <vector>
#include <DirectXMath.h>
#include "ConstantBuffers.h"

struct ID3D11ShaderResourceView;
struct ID3D11InputLayout;
struct D3D11_INPUT_ELEMENT_DESC;
class CachedShaders;
struct ID3D11Buffer;

class Material
{
public:
    Material();
    ~Material(){}

    std::vector<ID3D11ShaderResourceView*> Textures;
    std::string ShaderPath;
    std::vector<D3D11_INPUT_ELEMENT_DESC> Layout;
    DirectX::XMFLOAT3 Diffuse;
    DirectX::XMFLOAT3 Specular;

    const CachedShaders* GetShaders();
    ID3D11InputLayout* GetInputLayout();
    ID3D11Buffer* GetConstantBufferMaterialBuffer();

private:
    uint64_t m_shaderLastModTime;
    ID3D11InputLayout* m_inputLayout;

    const CachedShaders* m_shaders;
    
    cbMaterial m_cbMaterial;
    ID3D11Buffer* m_cbMaterialBuff;
};


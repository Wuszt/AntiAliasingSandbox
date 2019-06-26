#pragma once
#include <unordered_map>

struct ID3D11PixelShader;
struct ID3D11VertexShader;
struct ID3D11Device;
struct ID3D10Blob;
struct ID3D11InputLayout;

template<class T>
struct CompiledShader
{
    T* Shader;
    ID3D10Blob* ByteCode;
};

struct CachedShaders
{
    uint64_t LastModificationTime;
    CompiledShader<ID3D11VertexShader> VS;
    CompiledShader<ID3D11PixelShader> PS;

    ID3D11InputLayout* inputLayout;
};

class ShadersManager
{
public:
    ShadersManager(ID3D11Device* const& device);
    ~ShadersManager();

    const CachedShaders* GetShaders(const std::string& path);

private:

    ID3D11Device* m_device;
    std::unordered_map<std::string, CachedShaders> m_cachedShaders;
    
    void ReleaseShader(CachedShaders& cachedShader);
};


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
    std::string ErrorMsg;
    uint64_t LastModificationTime;
    CompiledShader<ID3D11VertexShader> VS;
    CompiledShader<ID3D11PixelShader> PS;
};

class ShadersManager
{
public:
    static void Initialize(ID3D11Device* const& device);
    static void Release();

    const CachedShaders* GetShaders(const std::string& path);

    inline static ShadersManager* GetShadersManager() { return s_instance; }
private:
    ShadersManager(ID3D11Device* const& device);
    ~ShadersManager();

    ID3D11Device* m_device;
    std::unordered_map<std::string, CachedShaders> m_cachedShaders;
    
    void ReleaseShader(CachedShaders& cachedShader);
    static ShadersManager* s_instance;
};


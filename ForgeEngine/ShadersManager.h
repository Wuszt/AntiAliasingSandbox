#pragma once
#include <unordered_map>

struct ID3D11PixelShader;
struct ID3D11VertexShader;
struct ID3D10Blob;
struct ID3D11InputLayout;
struct _WIN32_FILE_ATTRIBUTE_DATA;
typedef _WIN32_FILE_ATTRIBUTE_DATA WIN32_FILE_ATTRIBUTE_DATA;

template<class T>
struct CompiledShader
{
    T* Shader;
    ID3D10Blob* ByteCode;
};

class CachedShaders
{
public:
    const CompiledShader<ID3D11VertexShader>& GetVS() const { return m_vs; }
    const CompiledShader<ID3D11PixelShader>& GetPS() const { return m_ps; }
    uint64_t GetLastModificationTime() const { return m_lastModificationTime; }

private:

    friend class ShadersManager;

    std::string m_errorMsg;
    uint64_t m_lastModificationTime;
    CompiledShader<ID3D11VertexShader> m_vs;
    CompiledShader<ID3D11PixelShader> m_ps;
};

class ShadersManager
{
public:
    static void Initialize();
    static void Update();
    static void Release();

    const CachedShaders* GetShaders(const std::string& path);

    inline static ShadersManager* GetShadersManager() { return s_instance; }
private:
    ShadersManager();
    ~ShadersManager();

    std::unordered_map<std::string, CachedShaders*> m_cachedShaders;
    
    void OnUpdate();
    void ReleaseShaders(CachedShaders* cachedShaders);
    static ShadersManager* s_instance;

    bool TryToCompileShaders(std::string path, CachedShaders* destination);
    void GetCompiledShadersOrPlaceholder(std::string path, CachedShaders* destination);
    void GetCompiledShadersOrPlaceholder(std::string path, uint64_t modificationTime, CachedShaders* destination);

    uint64_t GetEncodedLastModificationTimeOfFile(std::string path);
};


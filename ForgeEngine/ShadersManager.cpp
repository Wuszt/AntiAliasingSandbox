#include "ShadersManager.h"
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "DebugLog.h"
#include "Core.h"

using namespace std;

ShadersManager::ShadersManager()
{
}

ShadersManager::~ShadersManager()
{
    for (auto& pair : m_cachedShaders)
    {
        ReleaseShaders(pair.second);
        delete pair.second;
    }
}

void ShadersManager::Initialize()
{
    s_instance = new ShadersManager();
}

void ShadersManager::Update()
{
    s_instance->OnUpdate();
}

void ShadersManager::Release()
{
    delete s_instance;
}

const CachedShaders* ShadersManager::GetShaders(const string& path)
{
    auto found = m_cachedShaders.find(path);

    if (found != m_cachedShaders.end())
    {
        return found->second;
    }

    CachedShaders* cached = new CachedShaders();
    GetCompiledShadersOrPlaceholder(path, cached);
    m_cachedShaders.emplace(path, cached);

    return cached;

        //GetFileAttributesEx(path.c_str(), GetFileExInfoStandard, &fInfo);
        //uint64_t lastMod = fInfo.ftLastWriteTime.dwHighDateTime;
        //lastMod = lastMod << 32 | fInfo.ftLastWriteTime.dwLowDateTime;

        //if (lastMod == cached->m_lastModificationTime)
        //{
           // if (cached->m_errorMsg == "")
            //    return cached;

            //DebugLog::LogError(cached->m_errorMsg);
          //  return GetShaders("Placeholder.fx");
     //   }
  //  }

    //if (cached == nullptr)
    //    cached = m_cachedShaders.insert({ path, new CachedShaders() }).first->second;
    //else if (cached->m_errorMsg == "")
    //    ReleaseShaders(cached);

    //GetFileAttributesEx(path.c_str(), GetFileExInfoStandard, &fInfo);
    //cached->m_lastModificationTime = fInfo.ftLastWriteTime.dwHighDateTime;
    //cached->m_lastModificationTime = cached->m_lastModificationTime << 32 | fInfo.ftLastWriteTime.dwLowDateTime;

    //cached->m_errorMsg = "";

    //if (!TryToCompileShaders(path, cached))
    //{
    //    DebugLog::LogError(cached->m_errorMsg);
    //    return GetShaders("Placeholder.fx");
    //}

    //return cached;
}

void ShadersManager::OnUpdate()
{
    for (auto& shaders : m_cachedShaders)
    {
        uint64_t lastMod = GetEncodedLastModificationTimeOfFile(shaders.first);

        if(!shaders.second->m_errorMsg.empty())
            DebugLog::LogError(shaders.second->m_errorMsg);

        if (lastMod != shaders.second->m_lastModificationTime)
        {
            GetCompiledShadersOrPlaceholder(shaders.first, lastMod, shaders.second);
        }
    }
}

void ShadersManager::ReleaseShaders(CachedShaders* cachedShaders)
{
    if (cachedShaders->GetPS().ByteCode)
        cachedShaders->GetPS().ByteCode->Release();

    if (cachedShaders->GetPS().Shader)
        cachedShaders->GetPS().Shader->Release();

    if (cachedShaders->GetVS().ByteCode)
        cachedShaders->GetVS().ByteCode->Release();

    if (cachedShaders->GetVS().Shader)
        cachedShaders->GetVS().Shader->Release();
}

ShadersManager* ShadersManager::s_instance;

bool ShadersManager::TryToCompileShaders(std::string path, CachedShaders* destination)
{
    HRESULT hr;

    do
    {
        ID3DBlob* errorMessages = nullptr;
        hr = D3DCompileFromFile(std::wstring(path.begin(), path.end()).c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, 0, &destination->m_vs.ByteCode, &errorMessages);

        if (hr == S_OK)
            hr = D3DCompileFromFile(std::wstring(path.begin(), path.end()).c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, 0, &destination->m_ps.ByteCode, &errorMessages);

        if (hr != S_OK && hr != HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION))
        {
            if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
                destination->m_errorMsg = "Couldn't find shader with path: " + path;
            else if (hr == E_FAIL)
            {
                destination->m_errorMsg = reinterpret_cast<const char*>(errorMessages->GetBufferPointer());
                errorMessages->Release();
            }
            else
                destination->m_errorMsg = "Unknown error while compiling " + path;

            return false;
        }

    } while (hr == HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION));

    Core::GetD3Device()->CreateVertexShader(destination->m_vs.ByteCode->GetBufferPointer(), destination->m_vs.ByteCode->GetBufferSize(), NULL, &destination->m_vs.Shader);
    Core::GetD3Device()->CreatePixelShader(destination->m_ps.ByteCode->GetBufferPointer(), destination->m_ps.ByteCode->GetBufferSize(), NULL, &destination->m_ps.Shader);

    destination->m_errorMsg = "";

    return true;
}

void ShadersManager::GetCompiledShadersOrPlaceholder(std::string path, CachedShaders* destination)
{
    uint64_t lastMod = GetEncodedLastModificationTimeOfFile(path);
    GetCompiledShadersOrPlaceholder(path, lastMod, destination);
}

void ShadersManager::GetCompiledShadersOrPlaceholder(std::string path, uint64_t modificationTime, CachedShaders* destination)
{
    destination->m_errorMsg = "";
    destination->m_lastModificationTime = modificationTime;
    if (!TryToCompileShaders(path, destination))
    {
        std::string errorMsg = destination->m_errorMsg;
        TryToCompileShaders("Placeholder.fx", destination);
        destination->m_errorMsg = errorMsg;
    }
}

uint64_t ShadersManager::GetEncodedLastModificationTimeOfFile(std::string path)
{
    static WIN32_FILE_ATTRIBUTE_DATA fInfo;

    GetFileAttributesEx(path.c_str(), GetFileExInfoStandard, &fInfo);
    uint64_t lastMod = fInfo.ftLastWriteTime.dwHighDateTime;
    lastMod = lastMod << 32 | fInfo.ftLastWriteTime.dwLowDateTime;

    return lastMod;
}

#include "ShadersManager.h"
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>

using namespace std;


ShadersManager::ShadersManager(ID3D11Device* const& device)
{
    m_device = device;
}

ShadersManager::~ShadersManager()
{
}

const CachedShaders* ShadersManager::GetShaders(const string& path)
{
    auto found = m_cachedShaders.find(path);

    static WIN32_FILE_ATTRIBUTE_DATA fInfo;
    static ID3D10Blob* buffer;

    CachedShaders* cached = nullptr;

    if (found != m_cachedShaders.end())
    {
        cached = &found->second;

        GetFileAttributesEx(path.c_str(), GetFileExInfoStandard, &fInfo);
        uint64_t lastMod = fInfo.ftLastWriteTime.dwHighDateTime;
        lastMod = lastMod << 32 | fInfo.ftLastWriteTime.dwLowDateTime;

        if (lastMod == cached->LastModificationTime)
            return cached;
    }


    if (cached == nullptr)
        cached = &m_cachedShaders.insert({ path, CachedShaders() }).first->second;

    bool firstTry = false;
    while (true)
    {
        HRESULT hr = D3DCompileFromFile(std::wstring(path.begin(), path.end()).c_str(), 0, 0, "VS", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, 0, &cached->VS.ByteCode, 0);

        if (hr == S_OK)
            break;

        if (firstTry)
            Sleep(100);
        else
            MessageBox(0, (string("VS ") + path + string(" compilation error")).c_str(), "Error", MB_OK);
    }

    firstTry = false;
    while (true)
    {
        HRESULT hr = D3DCompileFromFile(std::wstring(path.begin(), path.end()).c_str(), 0, 0, "PS", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, 0, &cached->PS.ByteCode, 0);

        if (hr == S_OK)
            break;

        if (firstTry)
            Sleep(100);
        else
            MessageBox(0, (string("PS ") + path + string(" compilation error")).c_str(), "Error", MB_OK);
    }

    m_device->CreateVertexShader(cached->VS.ByteCode->GetBufferPointer(), cached->VS.ByteCode->GetBufferSize(), NULL, &cached->VS.Shader);
    m_device->CreatePixelShader(cached->PS.ByteCode->GetBufferPointer(), cached->PS.ByteCode->GetBufferSize(), NULL, &cached->PS.Shader);

    GetFileAttributesEx(path.c_str(), GetFileExInfoStandard, &fInfo);
    cached->LastModificationTime = fInfo.ftLastWriteTime.dwHighDateTime;
    cached->LastModificationTime = cached->LastModificationTime << 32 | fInfo.ftLastWriteTime.dwLowDateTime;

    return cached;
}

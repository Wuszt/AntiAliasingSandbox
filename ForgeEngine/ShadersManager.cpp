#include "ShadersManager.h"
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>

using namespace std;

D3D11_INPUT_ELEMENT_DESC layout[2] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
UINT numElements = ARRAYSIZE(layout);


ShadersManager::ShadersManager(ID3D11Device* const& device)
{
    m_device = device;
}

ShadersManager::~ShadersManager()
{
    for (auto& pair : m_cachedShaders)
    {
        ReleaseShader(pair.second);
    }
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
    else
        ReleaseShader(*cached);

    while (true)
    {
        HRESULT hr = D3DCompileFromFile(std::wstring(path.begin(), path.end()).c_str(), 0, 0, "VS", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, 0, &cached->VS.ByteCode, 0);

        if (hr == S_OK)
            break;
        else if (hr == HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION))
        {
            Sleep(100);
            continue;
        }

        MessageBox(0, (string("VS ") + path + string(" compilation error")).c_str(), "Error", MB_OK);
    }

    while (true)
    {
        HRESULT hr = D3DCompileFromFile(std::wstring(path.begin(), path.end()).c_str(), 0, 0, "PS", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, 0, &cached->PS.ByteCode, 0);

        if (hr == S_OK)
            break;
        else if (hr == HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION))
        {
            Sleep(100);
            continue;
        }

        MessageBox(0, (string("PS ") + path + string(" compilation error")).c_str(), "Error", MB_OK);
    }

    m_device->CreateVertexShader(cached->VS.ByteCode->GetBufferPointer(), cached->VS.ByteCode->GetBufferSize(), NULL, &cached->VS.Shader);
    m_device->CreatePixelShader(cached->PS.ByteCode->GetBufferPointer(), cached->PS.ByteCode->GetBufferSize(), NULL, &cached->PS.Shader);

    m_device->CreateInputLayout(layout, numElements, cached->VS.ByteCode->GetBufferPointer(),
        cached->VS.ByteCode->GetBufferSize(), &cached->inputLayout);

    GetFileAttributesEx(path.c_str(), GetFileExInfoStandard, &fInfo);
    cached->LastModificationTime = fInfo.ftLastWriteTime.dwHighDateTime;
    cached->LastModificationTime = cached->LastModificationTime << 32 | fInfo.ftLastWriteTime.dwLowDateTime;

    return cached;
}

void ShadersManager::ReleaseShader(CachedShaders& cachedShader)
{
    cachedShader.inputLayout->Release();
    cachedShader.PS.ByteCode->Release();
    cachedShader.PS.Shader->Release();

    cachedShader.VS.ByteCode->Release();
    cachedShader.VS.Shader->Release();
}

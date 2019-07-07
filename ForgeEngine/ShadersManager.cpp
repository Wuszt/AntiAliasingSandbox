#include "ShadersManager.h"
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "DebugLog.h"

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

void ShadersManager::Initialize(ID3D11Device* const& device)
{
    s_instance = new ShadersManager(device);
}

void ShadersManager::Release()
{
    delete s_instance;
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
        {
            if (cached->ErrorMsg == "")
                return cached;

            DebugLog::LogError(cached->ErrorMsg);
            return GetShaders("Placeholder.fx");
        }
    }

    if (cached == nullptr)
        cached = &m_cachedShaders.insert({ path, CachedShaders() }).first->second;
    else if (cached->ErrorMsg == "")
        ReleaseShader(*cached);

    GetFileAttributesEx(path.c_str(), GetFileExInfoStandard, &fInfo);
    cached->LastModificationTime = fInfo.ftLastWriteTime.dwHighDateTime;
    cached->LastModificationTime = cached->LastModificationTime << 32 | fInfo.ftLastWriteTime.dwLowDateTime;

    cached->ErrorMsg = "";

    HRESULT hr;

    do
    {
        ID3DBlob* errorMessages = nullptr;
        hr = D3DCompileFromFile(std::wstring(path.begin(), path.end()).c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, 0, &cached->VS.ByteCode, &errorMessages);

        if (hr == S_OK)
            hr = D3DCompileFromFile(std::wstring(path.begin(), path.end()).c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, 0, &cached->PS.ByteCode, &errorMessages);

        if (hr != S_OK && hr != HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION))
        {
            if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
                cached->ErrorMsg = "Couldn't find shader with path: " + path;
            else if (hr == E_FAIL)
            {
                cached->ErrorMsg = reinterpret_cast<const char*>(errorMessages->GetBufferPointer());
                errorMessages->Release();
            }
            else
                cached->ErrorMsg = "Unknown error while compiling " + path;

            DebugLog::LogError(cached->ErrorMsg);

            return GetShaders("Placeholder.fx");
        }

    } while (hr == HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION));

    m_device->CreateVertexShader(cached->VS.ByteCode->GetBufferPointer(), cached->VS.ByteCode->GetBufferSize(), NULL, &cached->VS.Shader);
    m_device->CreatePixelShader(cached->PS.ByteCode->GetBufferPointer(), cached->PS.ByteCode->GetBufferSize(), NULL, &cached->PS.Shader);

    m_device->CreateInputLayout(layout, numElements, cached->VS.ByteCode->GetBufferPointer(),
        cached->VS.ByteCode->GetBufferSize(), &cached->inputLayout);

    cached->ErrorMsg = "";

    return cached;
}

void ShadersManager::ReleaseShader(CachedShaders& cachedShader)
{
    if (cachedShader.inputLayout)
        cachedShader.inputLayout->Release();

    if (cachedShader.PS.ByteCode)
        cachedShader.PS.ByteCode->Release();

    if (cachedShader.PS.Shader)
        cachedShader.PS.Shader->Release();

    if (cachedShader.VS.ByteCode)
        cachedShader.VS.ByteCode->Release();

    if (cachedShader.VS.Shader)
        cachedShader.VS.Shader->Release();
}

ShadersManager* ShadersManager::s_instance;

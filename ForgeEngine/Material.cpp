#include "Material.h"
#include "ShadersManager.h"
#include "Core.h"
#include <exception>
#include <comdef.h>

Material::Material()
{
    D3D11_BUFFER_DESC cbbd;
    ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

    cbbd.Usage = D3D11_USAGE_DEFAULT;
    cbbd.ByteWidth = sizeof(cbMaterial);
    cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    HRESULT hr = Core::GetD3Device()->CreateBuffer(&cbbd, nullptr, &m_cbMaterialBuff);

    if (hr != S_OK)
        throw std::exception();
}

const CachedShaders* Material::GetShaders()
{
    if (m_shaders != nullptr)
        return m_shaders;

    m_shaders = ShadersManager::GetShadersManager()->GetShaders(ShaderPath.empty() ? "Base.fx" : ShaderPath);

    return m_shaders;
}

ID3D11InputLayout* Material::GetInputLayout()
{
    static const CachedShaders* shaders;
    shaders = GetShaders();

    if (shaders->GetLastModificationTime() != m_shaderLastModTime)
    {
        if (m_inputLayout)
            m_inputLayout->Release();

        Core::GetD3Device()->CreateInputLayout(Layout.data(), (UINT)Layout.size(), shaders->GetVS().ByteCode->GetBufferPointer(),
            shaders->GetVS().ByteCode->GetBufferSize(), &m_inputLayout);

        m_shaderLastModTime = shaders->GetLastModificationTime();
    }
    return m_inputLayout;
}

ID3D11Buffer* Material::GetConstantBufferMaterialBuffer()
{
    m_cbMaterial.Diffuse = Diffuse;
    m_cbMaterial.Specular = Specular;
    Core::GetD3DeviceContext()->UpdateSubresource(m_cbMaterialBuff, 0, nullptr, &m_cbMaterial, 0, 0);

    return m_cbMaterialBuff;
}

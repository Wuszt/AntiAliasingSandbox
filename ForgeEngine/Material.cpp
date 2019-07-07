#include "Material.h"
#include "ShadersManager.h"
#include "Core.h"

const CachedShaders* Material::GetShaders() const
{
    return ShadersManager::GetShadersManager()->GetShaders(ShaderPath.empty() ? "Base.fx" : ShaderPath);
}

ID3D11InputLayout* Material::GetInputLayout()
{
    static const CachedShaders* shaders;
    shaders = GetShaders();

    if (shaders->LastModificationTime != m_shaderLastModTime)
    {
        if (m_inputLayout)
            m_inputLayout->Release();

        Core::GetD3Device()->CreateInputLayout(Layout.data(), (UINT)Layout.size(), shaders->VS.ByteCode->GetBufferPointer(),
            shaders->VS.ByteCode->GetBufferSize(), &m_inputLayout);

        m_shaderLastModTime = shaders->LastModificationTime;
    }

    return m_inputLayout;
}

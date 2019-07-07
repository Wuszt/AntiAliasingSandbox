#include "PostProcessor.h"
#include "ShadersManager.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXTex/DirectXTex.h>

using namespace DirectX;

struct Vertex //to move
{
    Vertex() {}
    Vertex(const float& x, const float& y, const float& z,
        const float& u, const float& v)
        : pos(x, y, z), texCoord(u, v) {}

    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT2 texCoord;
};

D3D11_INPUT_ELEMENT_DESC layout[2] =
{
    { "SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
UINT numElements = ARRAYSIZE(layout);

PostProcessor::PostProcessor(ID3D11Device* const& device, ID3D11DeviceContext* const& context)
{
    m_d3Device = device;
    m_d3Context = context;

    const CachedShaders* basePPShader = ShadersManager::GetShadersManager()->GetShaders("CopyingPP.fx");

    m_d3Device->CreateInputLayout(layout, numElements, basePPShader->VS.ByteCode->GetBufferPointer(),
        basePPShader->VS.ByteCode->GetBufferSize(), &s_inputLayout);
}

PostProcessor::~PostProcessor()
{
}

void PostProcessor::DrawPass(ID3D11Device* const& device, ID3D11DeviceContext* const& context, const std::string& shaderName, const std::vector<ID3D11Texture2D*>& textures, ID3D11RenderTargetView* const& target)
{
    context->OMSetRenderTargets(1, &target, nullptr);
    const CachedShaders* shader = ShadersManager::GetShadersManager()->GetShaders(shaderName);

    static float bgColor[4] = { (0.0f, 0.0f, 0.0f, 0.0f) };
    context->ClearRenderTargetView(target, bgColor);

    D3D11_SHADER_RESOURCE_VIEW_DESC shDesc;
    shDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    shDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shDesc.Texture2D.MostDetailedMip = 0;
    shDesc.Texture2D.MipLevels = 1;

    for (int i = 0; i < textures.size(); ++i)
    {
        ID3D11ShaderResourceView* rv = nullptr;

        device->CreateShaderResourceView(textures[i], &shDesc, &rv);

        context->PSSetShaderResources(i, 1, &rv);

        rv->Release();
    }

    context->VSSetShader(shader->VS.Shader, nullptr, 0);
    context->PSSetShader(shader->PS.Shader, nullptr, 0);

    static Vertex Vertices[6] =
    {
        {-1.0f, -1.0f, 0, 0.0f, 1.0f},
        {-1.0f, 1.0f, 0, 0.0f, 0.0f},
        {1.0f, -1.0f, 0, 1.0f, 1.0f},
        {1.0f, -1.0f, 0, 1.0f, 1.0f},
        {-1.0f, 1.0f, 0, 0.0f, 0.0f},
        {1.0f, 1.0f, 0, 1.0f, 0.0f},
    };

    D3D11_BUFFER_DESC BufferDesc;
    RtlZeroMemory(&BufferDesc, sizeof(BufferDesc));
    BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    BufferDesc.ByteWidth = sizeof(Vertex) * 6;
    BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    BufferDesc.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
    RtlZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = Vertices;

    ID3D11Buffer* VertexBuffer = nullptr;

    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    auto tmp1 = device->CreateBuffer(&BufferDesc, &InitData, &VertexBuffer);
    context->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);

    context->IASetInputLayout(s_inputLayout);

    context->Draw(6, 0);

    VertexBuffer->Release();
}

void PostProcessor::DrawPass(const std::string& shaderName, const std::vector<ID3D11Texture2D*>& textures, ID3D11RenderTargetView* const& target)
{
    PostProcessor::DrawPass(m_d3Device, m_d3Context, shaderName, textures, target);
}

ID3D11InputLayout* PostProcessor::s_inputLayout;

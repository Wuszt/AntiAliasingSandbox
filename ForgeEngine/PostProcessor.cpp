#include "PostProcessor.h"
#include "ShadersManager.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXTex/DirectXTex.h>
#include "Core.h"

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

void PostProcessor::Initialize()
{
    const CachedShaders* basePPShader = ShadersManager::GetShadersManager()->GetShaders("CopyingPP.fx");

    Core::GetD3Device()->CreateInputLayout(layout, numElements, basePPShader->VS.ByteCode->GetBufferPointer(),
        basePPShader->VS.ByteCode->GetBufferSize(), &s_inputLayout);
}

void PostProcessor::Release()
{
    s_inputLayout->Release();
}

void PostProcessor::DrawPass(const std::string& shaderName, const std::vector<ID3D11Texture2D*>& textures, ID3D11RenderTargetView* const& target)
{
    Core::GetD3DeviceContext()->OMSetRenderTargets(1, &target, nullptr);
    const CachedShaders* shader = ShadersManager::GetShadersManager()->GetShaders(shaderName);

    static float bgColor[4] = { (0.0f, 0.0f, 0.0f, 0.0f) };
    Core::GetD3DeviceContext()->ClearRenderTargetView(target, bgColor);

    D3D11_SHADER_RESOURCE_VIEW_DESC shDesc;
    shDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    shDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shDesc.Texture2D.MostDetailedMip = 0;
    shDesc.Texture2D.MipLevels = 1;

    for (int i = 0; i < textures.size(); ++i)
    {
        ID3D11ShaderResourceView* rv = nullptr;

        Core::GetD3Device()->CreateShaderResourceView(textures[i], &shDesc, &rv);

        Core::GetD3DeviceContext()->PSSetShaderResources(i, 1, &rv);

        rv->Release();
    }

    Core::GetD3DeviceContext()->VSSetShader(shader->VS.Shader, nullptr, 0);
    Core::GetD3DeviceContext()->PSSetShader(shader->PS.Shader, nullptr, 0);

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

    auto tmp1 = Core::GetD3Device()->CreateBuffer(&BufferDesc, &InitData, &VertexBuffer);
    Core::GetD3DeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);

    Core::GetD3DeviceContext()->IASetInputLayout(s_inputLayout);

    Core::GetD3DeviceContext()->Draw(6, 0);

    VertexBuffer->Release();
}

ID3D11InputLayout* PostProcessor::s_inputLayout;

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

PostProcessor::PostProcessor(ID3D11Device* const& device, ID3D11DeviceContext* const& context, ShadersManager* const& shadersManager)
{
    m_shadersManager = shadersManager;
    m_d3Device = device;
    m_d3Context = context;
}

PostProcessor::~PostProcessor()
{
}

void PostProcessor::DrawPass(const std::string& shaderName, const std::vector<ID3D11Texture2D*>& textures, ID3D11RenderTargetView* const& target)
{
    m_d3Context->OMSetRenderTargets(1, &target, nullptr);
    const CachedShaders* shader = m_shadersManager->GetShaders(shaderName);

    static float bgColor[4] = { (0.0f, 0.0f, 0.0f, 0.0f) };
    m_d3Context->ClearRenderTargetView(target, bgColor);

    std::vector<ID3D11ShaderResourceView*> resources;

    D3D11_SHADER_RESOURCE_VIEW_DESC shDesc;
    shDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    shDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shDesc.Texture2D.MostDetailedMip = 0;
    shDesc.Texture2D.MipLevels = 1;

    for (int i = 0; i < textures.size(); ++i)
    {
        ID3D11ShaderResourceView* rv = nullptr;

        m_d3Device->CreateShaderResourceView(textures[i], &shDesc, &rv);

        resources.push_back(rv);

        m_d3Context->PSSetShaderResources(i, 1, &rv);
    }

    m_d3Context->VSSetShader(shader->VS.Shader, nullptr, 0);
    m_d3Context->PSSetShader(shader->PS.Shader, nullptr, 0);

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

    auto tmp1 = m_d3Device->CreateBuffer(&BufferDesc, &InitData, &VertexBuffer);
    m_d3Context->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);


    m_d3Context->Draw(6, 0);

    for (auto& rv : resources)
    {
        rv->Release();
    }

    VertexBuffer->Release();
}

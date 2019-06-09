#pragma once
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

class Window;
class Camera;

class Core
{
public:
    Core(const HINSTANCE& hInstance, const int& ShowWnd, const int& width, const int& height);
    virtual ~Core();

    void Run();

protected:
    virtual HRESULT InitializeD3D(const HINSTANCE& hInstance);
    virtual HRESULT InitializeSwapChain();
    virtual HRESULT InitializeDepthStencilBuffer();

    int m_width;
    int m_height;

private:
    void FillDepthStencilDescWithDefaultValues(D3D11_TEXTURE2D_DESC& desc);
    void FillSwapChainBufferDescWithDefaultValues(DXGI_MODE_DESC& desc);
    void FillSwapChainDescWithDefaultValues(DXGI_SWAP_CHAIN_DESC& desc);

    bool InitScene();
    void UpdateScene();
    void DrawScene();

    IDXGISwapChain* m_swapChain;
    ID3D11Device* m_d3Device;
    ID3D11DeviceContext* m_d3DeviceContext;
    ID3D11RenderTargetView* m_renderTargetView;
    ID3D11DepthStencilView* m_depthStencilView;
    ID3D11Texture2D* m_depthStencilBuffer;

    Window* m_window;
    Camera* m_camera;

    //to move
    ID3D11VertexShader* VS;
    ID3D11PixelShader* PS;
    ID3D10Blob* VS_Buffer;
    ID3D10Blob* PS_Buffer;
    ID3D11InputLayout* vertLayout;
    ID3D11Buffer* cbPerObjectBuffer;

    ID3D11Buffer* VertBuffer;
    ID3D11Buffer* IndexBuffer;

    struct Vertex
    {
        Vertex() {}
        Vertex(const float& x, const float& y, const float& z,
            const float& cr, const float& cg, const float& cb, const float& ca)
            : pos(x, y, z), color(cr, cg, cb, ca) {}

        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT4 color;
    };

    struct cbPerObject
    {
        DirectX::XMMATRIX WVP;
    };

    cbPerObject cbPerObj;

    D3D11_INPUT_ELEMENT_DESC layout[2] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);

    //end to move
};


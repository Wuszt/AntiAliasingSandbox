#pragma once
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

class Window;

class Core
{
public:
    Core(HINSTANCE hInstance, int ShowWnd, int width, int height);
    virtual ~Core();

    void Run();

    bool InitializeDirect3d11App(HINSTANCE hInstance);
    bool InitScene();
    void UpdateScene();
    void DrawScene();

private:
    int m_width;
    int m_height;

    IDXGISwapChain* SwapChain;
    ID3D11Device* d3d11Device;
    ID3D11DeviceContext* d3d11DevCon;
    ID3D11RenderTargetView* renderTargetView;

    ID3D11Buffer* VertBuffer;
    ID3D11Buffer* IndexBuffer;
    ID3D11DepthStencilView* depthStencilView;
    ID3D11Texture2D* depthStencilBuffer;

    ID3D11VertexShader* VS;
    ID3D11PixelShader* PS;
    ID3D10Blob* VS_Buffer;
    ID3D10Blob* PS_Buffer;
    ID3D11InputLayout* vertLayout;
    ID3D11Buffer* cbPerObjectBuffer;

    HRESULT hr;

    DirectX::XMMATRIX WVP;
    DirectX::XMMATRIX World;
    DirectX::XMMATRIX camView;
    DirectX::XMMATRIX camProjection;

    DirectX::XMVECTOR camPos;
    DirectX::XMVECTOR camLookAt;
    DirectX::XMVECTOR camUp;

    Window* m_window;

    struct Vertex	//Overloaded Vertex Structure
    {
        Vertex() {}
        Vertex(float x, float y, float z,
            float cr, float cg, float cb, float ca)
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
};


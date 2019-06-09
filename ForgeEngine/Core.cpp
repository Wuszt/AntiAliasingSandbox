#include "Core.h"
#include "Window.h"
#include <exception>

using namespace DirectX;

Core::Core(HINSTANCE hInstance, int ShowWnd, int width, int height)
{
    m_window = new Window(hInstance, ShowWnd, width, height, true);
    m_width = width;
    m_height = height;


    if (!InitializeDirect3d11App(hInstance))
    {
        MessageBox(0, "Direct3D Initialization - Failed",
            "Error", MB_OK);
        throw std::exception("Direct3D Initialization - Failed");
    }

    if (!InitScene())
    {
        MessageBox(0, "Scene Initialization - Failed",
            "Error", MB_OK);
        throw std::exception("Scene Initialization - Failed");
    }
}

Core::~Core()
{
}

void Core::Run()
{
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));
    while (true)
    {
        BOOL PeekMessageL(
            LPMSG lpMsg,
            HWND hWnd,
            UINT wMsgFilterMin,
            UINT wMsgFilterMax,
            UINT wRemoveMsg
        );

        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            // run game code            
            UpdateScene();
            DrawScene();
        }
    }
}

bool Core::InitializeDirect3d11App(HINSTANCE hInstance)
{
    DXGI_MODE_DESC bufferDesc;

    ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

    bufferDesc.Width = m_width;
    bufferDesc.Height = m_height;
    bufferDesc.RefreshRate.Numerator = 60;
    bufferDesc.RefreshRate.Denominator = 1;
    bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    //Describe our SwapChain
    DXGI_SWAP_CHAIN_DESC swapChainDesc;

    ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

    swapChainDesc.BufferDesc = bufferDesc;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.OutputWindow = *(m_window->GetHWND());
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;


    //Create our SwapChain
    hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
        D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &d3d11Device, NULL, &d3d11DevCon);

    //Create our BackBuffer
    ID3D11Texture2D* BackBuffer;
    hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);

    //Create our Render Target
    hr = d3d11Device->CreateRenderTargetView(BackBuffer, NULL, &renderTargetView);
    BackBuffer->Release();

    D3D11_TEXTURE2D_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

    depthStencilDesc.Width = m_width;
    depthStencilDesc.Height = m_height;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    d3d11Device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilBuffer);
    d3d11Device->CreateDepthStencilView(depthStencilBuffer, nullptr, &depthStencilView);

    //Set our Render Target
    d3d11DevCon->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

    return true;
}

bool Core::InitScene()
{
    //Compile Shaders from shader file
    hr = D3DCompileFromFile(L"Effects.fx", 0, 0, "VS", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, 0, &VS_Buffer, 0);
    hr = D3DCompileFromFile(L"Effects.fx", 0, 0, "PS", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG, 0, &PS_Buffer, 0);

    //Create the Shader Objects
    hr = d3d11Device->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &VS);
    hr = d3d11Device->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &PS);

    //Set Vertex and Pixel Shaders
    d3d11DevCon->VSSetShader(VS, 0, 0);
    d3d11DevCon->PSSetShader(PS, 0, 0);

    //Create the vertex buffer
    Vertex v[] =
    {
    Vertex(-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f),
    Vertex(-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f),
    Vertex(0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f),
    Vertex(0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f),

    Vertex(-0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f),
    Vertex(-0.5f,  0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f),
    Vertex(0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f),
    Vertex(0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f),
    };

    DWORD indices[] =
    {
        0,1,2,
        0,2,3,

        7,6,5,
        7,5,4,

        3,2,6,
        3,6,7,

        0,4,1,
        1,4,5,

        1,5,2,
        2,5,6,

        0,3,4,
        3,7,4
    };

    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = indices;

    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(DWORD) * 12 * 3; //indices amount
    d3d11Device->CreateBuffer(&indexBufferDesc, &iinitData, &IndexBuffer);
    d3d11DevCon->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * 8;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA vertexBufferData;

    ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
    vertexBufferData.pSysMem = v;
    hr = d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &VertBuffer);

    //Set the vertex buffer
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    d3d11DevCon->IASetVertexBuffers(0, 1, &VertBuffer, &stride, &offset);

    //Create the Input Layout
    hr = d3d11Device->CreateInputLayout(layout, numElements, VS_Buffer->GetBufferPointer(),
        VS_Buffer->GetBufferSize(), &vertLayout);

    //Set the Input Layout
    d3d11DevCon->IASetInputLayout(vertLayout);

    //Set Primitive Topology
    d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //Create the Viewport
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = m_width;
    viewport.Height = m_height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    //Set the Viewport
    d3d11DevCon->RSSetViewports(1, &viewport);

    D3D11_BUFFER_DESC cbbd;
    ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

    cbbd.Usage = D3D11_USAGE_DEFAULT;
    cbbd.ByteWidth = sizeof(cbPerObject);
    cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    hr = d3d11Device->CreateBuffer(&cbbd, nullptr, &cbPerObjectBuffer);

    camPos = XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f);
    camLookAt = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    camView = XMMatrixLookAtLH(camPos, camLookAt, camUp);

    camProjection = XMMatrixPerspectiveFovLH(0.4f * 3.14f, (float)m_width / m_height, 1.0f, 1000.0f);

    return true;
}

void Core::UpdateScene()
{

}

void Core::DrawScene()
{
    static float debug = 0;
    debug += 0.0001f;

    //Clear our backbuffer
    float bgColor[4] = { (0.0f, 0.0f, 0.0f, 0.0f) };
    d3d11DevCon->ClearRenderTargetView(renderTargetView, bgColor);

    d3d11DevCon->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    World = XMMatrixIdentity();
    World *= XMMatrixRotationX(debug);
    World *= XMMatrixRotationY(sinf(debug));

    WVP = World * camView * camProjection;
    cbPerObj.WVP = XMMatrixTranspose(WVP);

    d3d11DevCon->UpdateSubresource(cbPerObjectBuffer, 0, nullptr, &cbPerObj, 0, 0);

    d3d11DevCon->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);

    //Draw the triangle
    d3d11DevCon->DrawIndexed(36, 0, 0);

    //Present the backbuffer to the screen
    SwapChain->Present(0, 0);
}

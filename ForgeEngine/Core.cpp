#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Core.h"
#include "Window.h"
#include "ControllableCamera.h"
#include "Transform.h"
#include <DirectXCommonClasses/Time.h>
#include <DirectXCommonClasses/InputClass.h>
#include <exception>
#include "Object.h"
#include <DirectXTex/DirectXTex.h>

#include "RenderingSystem.h"
#include "MeshRenderer.h"
#include "ShadersManager.h"
#include "DebugLog.h"
#include "Profiler.h"
#include "RenderTargetViewsManager.h"
#include "PostProcessor.h"
#include "LightsManager.h"
#include "UIRenderingSystem.h"
#include <chrono>
#include <fileapi.h>
#include <winnt.h>
#include "DirectionalLight.h"

using namespace DirectX;

Core* Core::s_instance;

Core::Core()
{
    s_instance = this;
}

Core::~Core()
{
    for (Object* const& obj : m_objects)
        delete obj;

    for (Object* const& obj : m_objectsToAdd)
        delete obj;

    for (Object* const& obj : m_objectsToDelete)
        delete obj;

    m_swapChain->Release();
    m_d3Device->Release();
    m_d3DeviceContext->Release();
    m_renderTargetView->Release();
    m_depthStencilView->Release();
    m_rtvsManager->ReleaseRTV(m_temporaryRTV);
    m_rtvsManager->ReleaseRTV(m_velocityRTV);
    m_rasterizerState->Release();

    delete m_UIRenderingSystem;
    delete m_renderingSystem;
    delete m_window;
    delete m_lightsManager;
    delete m_depthStencilRTV;

    for (ID3D11SamplerState* const& sampler : m_samplerStates)
    {
        sampler->Release();
    }

    InputClass::Release();
    DebugLog::Release();

    PostProcessor::Release();

    ShadersManager::Release();
    delete m_rtvsManager;
}

void Core::Run(const HINSTANCE& hInstance, const int& ShowWnd, const int& width, const int& height, int resW, int resH, std::string resultsPath)
{
    Initialize(hInstance, ShowWnd, width, height);

    m_resultsPath = resultsPath;

    m_window->SetResolution(resW, resH);

    while (m_window->IsAlive())
    {
        Profiler::StartFrame();

        Profiler::StartProfiling(FRAME_ANALYZE_NAME);
        Profiler::StartCPUProfiling("Engine frame");

        m_temporaryRTV = GetRTVForTemporary();

        m_window->Update();
        Time::UpdateTime(false);
        InputClass::UpdateInput();
        ShadersManager::Update();
        BeforeUpdateScene();
        UpdateScene();
        AfterUpdateScene();

        DeletePendingObjects();
        AddPendingObjects();

        MainRTVProcessing();

        {
            Profiler::StartProfiling("UI");
            GetUIRenderingSystem()->OnBeforeDrawing();
            Profiler::StartProfiling("Profiler");
            Profiler::Draw();
            Profiler::EndProfiling("Profiler");

            Profiler::StartProfiling("DebugLog");
            DebugLog::Draw();
            Profiler::EndProfiling("DebugLog");

            Profiler::EndProfiling("UI");
        }

        MergeRTVsToMain();
        Profiler::StartProfiling("Empty");
        Profiler::EndProfiling("Empty");

        Profiler::StartProfiling("Swapchain");
        m_swapChain->Present(0, 0);
        Profiler::EndProfiling("Swapchain");

        Profiler::EndCPUProfiling("Engine frame");
        Profiler::EndProfiling(FRAME_ANALYZE_NAME);
        Profiler::EndFrame();

        m_rtvsManager->ReleaseRTV(m_temporaryRTV);

        if (m_isSSRequested)
        {
            MakeScreenshot(m_requestedSSFileName);
            m_isSSRequested = false;
        }
    }
}

void Core::MakeScreenshot(std::string name)
{
    DirectX::ScratchImage image;

    if (name == "")
    {
        auto p = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(p.time_since_epoch());
        name = std::to_string(ms.count());
    }

    std::string str = GetResultsPath() + "/" + name + ".png";

    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    _splitpath_s(str.c_str(), drive, _MAX_DRIVE, &dir[0], _MAX_DIR, nullptr, 0, nullptr, 0);
    std::string dirStr(dir);
    for (size_t i = dirStr.find('/', 1); i - 1 != std::string::npos; i = dirStr.find('/', i) + 1)
    {
        std::string driveDir = std::string(drive) + dirStr.substr(0, i);
        CreateDirectory(LPCSTR(driveDir .c_str()), NULL);
    }
    CreateDirectory(LPCSTR((std::string(drive) + dirStr).c_str()), NULL);

    std::wstring wstr(str.begin(), str.end());

    ID3D11Texture2D* BackBuffer;
    s_instance->m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);

    DirectX::CaptureTexture(GetD3Device(), GetD3DeviceContext(), BackBuffer, image);

    HRESULT hr = DirectX::SaveToWICFile(image.GetImages(), image.GetImageCount(), DDS_FLAGS_NONE, GetWICCodec(WIC_CODEC_PNG), wstr.c_str());
}

void Core::RequestScreenshot(std::string name /*= ""*/)
{
    s_instance->m_requestedSSFileName = name;
    s_instance->m_isSSRequested = true;
}

HRESULT Core::InitializeSwapChain()
{
    if (m_swapChain != nullptr)
        m_swapChain->Release();

    DXGI_MODE_DESC bufferDesc;
    FillSwapChainBufferDescWithDefaultValues(bufferDesc);

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    FillSwapChainDescWithDefaultValues(swapChainDesc);
    swapChainDesc.BufferDesc = bufferDesc;

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_1
    };

    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    return D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_d3Device, NULL, &m_d3DeviceContext);
}

HRESULT Core::InitializeDepthStencilBuffer()
{
    if (m_depthStencilBuffer != nullptr)
        m_depthStencilBuffer->Release();

    if (m_depthStencilView != nullptr)
        m_depthStencilView->Release();

    D3D11_TEXTURE2D_DESC depthStencilDesc;
    FillDepthStencilDescWithDefaultValues(depthStencilDesc);

    HRESULT hr = m_d3Device->CreateTexture2D(&depthStencilDesc, nullptr, &m_depthStencilBuffer);

    if (hr != S_OK)
        return hr;

    D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
    dsv_desc.Flags = 0;
    dsv_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsv_desc.ViewDimension = depthStencilDesc.SampleDesc.Count > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
    dsv_desc.Texture2D.MipSlice = 0;

    D3D11_SHADER_RESOURCE_VIEW_DESC sr_desc;
    sr_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    sr_desc.ViewDimension = depthStencilDesc.SampleDesc.Count > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
    sr_desc.Texture2D.MostDetailedMip = 0;
    sr_desc.Texture2D.MipLevels = -1;

    ID3D11ShaderResourceView* srv = nullptr;

    hr = m_d3Device->CreateDepthStencilView(m_depthStencilBuffer, &dsv_desc, &m_depthStencilView);
    hr = m_d3Device->CreateShaderResourceView(m_depthStencilBuffer, &sr_desc, &srv);

    m_depthStencilRTV = new RTV(nullptr, m_depthStencilBuffer, srv, 1, DXGI_FORMAT_R24G8_TYPELESS);

    return hr;
}

void Core::Initialize(const HINSTANCE& hInstance, const int& ShowWnd, const int& width, const int& height)
{
    m_window = new Window(hInstance, ShowWnd, width, height, true);
    m_window->AddResizeListener(Core::OnResizeCallback);
    m_window->AddResolutionChangeListener(Core::OnChangeResolutionCallback);

    if (InitializeD3D() != S_OK)
    {
        MessageBox(0, "Direct3D Initialization - Failed",
            "Error", MB_OK);
        throw std::exception("Direct3D Initialization - Failed");
    }

    ShadersManager::Initialize();
    m_rtvsManager = new RenderTargetViewsManager(m_window);
    m_renderingSystem = new RenderingSystem();
    m_UIRenderingSystem = new UIRenderingSystem();

    DebugLog::Initialize(m_window);

    PostProcessor::Initialize();
    m_lightsManager = new LightsManager();

    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    ID3D11SamplerState* samplerState;
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    if (S_OK != m_d3Device->CreateSamplerState(&sampDesc, &samplerState))
        throw std::exception("error");
    m_samplerStates.push_back(samplerState);

    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

    if (S_OK != m_d3Device->CreateSamplerState(&sampDesc, &samplerState))
        throw std::exception("error");
    m_samplerStates.push_back(samplerState);

    Time::Initialize();
    InputClass::Initialize(*m_window->GetHInstance(), *m_window->GetHWND());
    Profiler::Initialize(m_window);

    m_d3DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    InitScene();

    D3D11_BUFFER_DESC cbbd;

    ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
    cbbd.Usage = D3D11_USAGE_DEFAULT;
    cbbd.ByteWidth = sizeof(cbPerFrame);
    cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    m_d3Device->CreateBuffer(&cbbd, nullptr, &m_cbPerFrameBuff);

    ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
    cbbd.Usage = D3D11_USAGE_DEFAULT;
    cbbd.ByteWidth = sizeof(cbGlobalInfo);
    cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    m_d3Device->CreateBuffer(&cbbd, nullptr, &m_cbGlobalInfoBuff);

    ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
    cbbd.Usage = D3D11_USAGE_DEFAULT;
    cbbd.ByteWidth = sizeof(cbCameraInfo);
    cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    m_d3Device->CreateBuffer(&cbbd, nullptr, &m_cbCameraInfoBuff);

    D3D11_RASTERIZER_DESC rasterizerDesc;
    ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
    rasterizerDesc.AntialiasedLineEnable = false;
    rasterizerDesc.CullMode = D3D11_CULL_NONE;
    rasterizerDesc.FrontCounterClockwise = true;
    rasterizerDesc.DepthBias = 0;
    rasterizerDesc.DepthBiasClamp = 0.0f;
    rasterizerDesc.SlopeScaledDepthBias = 0.0f;
    rasterizerDesc.DepthClipEnable = true;
    rasterizerDesc.ScissorEnable = false;
    rasterizerDesc.MultisampleEnable = true;
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;

    m_d3Device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState);
    m_d3DeviceContext->RSSetState(m_rasterizerState);

    m_velocityRTV = m_rtvsManager->AcquireRTV(SizeType::Resolution, 1, DXGI_FORMAT_R16G16_SNORM);
}

HRESULT Core::InitializeD3D()
{
    if (m_renderTargetView != nullptr)
        m_renderTargetView->Release();

    InitializeSwapChain();
    ID3D11Texture2D* BackBuffer;
    HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);

    if (hr != S_OK)
        return hr;

    hr = m_d3Device->CreateRenderTargetView(BackBuffer, NULL, &m_renderTargetView);
    BackBuffer->Release();

    hr = InitializeDepthStencilBuffer();

    return hr;
}

void Core::FillDepthStencilDescWithDefaultValues(D3D11_TEXTURE2D_DESC& desc)
{
    ZeroMemory(&desc, sizeof(desc));

    desc.Width = m_window->GetResolutionWidth();
    desc.Height = m_window->GetResolutionHeight();
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
}

void Core::FillSwapChainBufferDescWithDefaultValues(DXGI_MODE_DESC& desc)
{
    ZeroMemory(&desc, sizeof(DXGI_MODE_DESC));

    desc.Width = m_window->GetWidth();
    desc.Height = m_window->GetHeight();
    desc.RefreshRate.Numerator = 0;
    desc.RefreshRate.Denominator = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
}

void Core::FillSwapChainDescWithDefaultValues(DXGI_SWAP_CHAIN_DESC& desc)
{
    ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));

    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 1;
    desc.OutputWindow = *(m_window->GetHWND());
    desc.Windowed = TRUE;
    desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
}

RTV* Core::GetRTVForTemporary()
{
    return m_rtvsManager->AcquireRTV(SizeType::Resolution);
}

void Core::AddPendingObjects()
{
    for (Object* const& object : m_objectsToAdd)
    {
        bool success = m_objects.insert(object).second;
        assert(success);
    }

    m_objectsToAdd.clear();
}

void Core::DeletePendingObjects()
{
    for (Object* const& object : m_objectsToDelete)
    {
        size_t removedElements = m_objects.erase(object);

        assert(removedElements == 1);

        delete object;
    }

    m_objectsToDelete.clear();
}

void Core::InitScene()
{
    m_camera = InstantiateObject<ControllableCamera>();
    m_camera->Initialize(0.4f * 3.14f, (float)m_window->GetWidth() / m_window->GetHeight(), 0.1f, 100.0f);
    m_camera->GetTransform()->SetPosition({ 0.0f, 0.0f, -5.0f });
    m_camera->GetTransform()->LookAt(XMFLOAT3(0.0f, 0.0f, 0.0f));

    for ( int x = 0; x < 4; ++x )
    {
        for ( int y = 0; y < 4; ++y )
        {
            auto m_car = InstantiateObject<Object>();
            m_car->GetTransform()->SetPosition( { x * 10.0f, 0.0f, y * 10.0f } );
            m_car->AddComponent<MeshRenderer>( "model.fbx", "Base.fx" );
        }
    }

    InstantiateObject<Object>()->AddComponent<DirectionalLight>( XMFLOAT3( 0.5f, -1.0f, 0.5f ), XMFLOAT3( 1.0f, 1.0f, 1.0f ), 1.0f );
}

void Core::BeforeUpdateScene()
{


}

void Core::UpdateScene()
{
    for (Object* obj : m_objects)
    {
        obj->Update();
    }
}

void Core::MainRTVProcessing()
{
    Profiler::StartProfiling("Drawing");
    DrawScene();
    Profiler::EndProfiling("Drawing");

    Profiler::StartProfiling("PostProcessing");
    PostProcessing();
    Profiler::EndProfiling("PostProcessing");
}

void Core::PostProcessing()
{

}

void Core::AfterUpdateScene()
{
    m_cbPerFrame.Time = Time::GetTime();
    m_d3DeviceContext->UpdateSubresource(m_cbPerFrameBuff, 0, nullptr, &m_cbPerFrame, 0, 0);

    m_d3DeviceContext->VSSetConstantBuffers(static_cast<UINT>(VertexCBIndex::PerFrame), 1, &m_cbPerFrameBuff);
    m_d3DeviceContext->PSSetConstantBuffers(static_cast<UINT>(PixelCBIndex::PerFrame), 1, &m_cbPerFrameBuff);

    m_cbGlobalInfo.Resolution = { (float)GetWindow()->GetResolutionWidth(), (float)GetWindow()->GetResolutionHeight() };
    m_cbGlobalInfo.TexelSize = { 1.0f / (float)GetWindow()->GetResolutionWidth(), 1.0f / (float)GetWindow()->GetResolutionHeight() };

    m_d3DeviceContext->UpdateSubresource(m_cbGlobalInfoBuff, 0, nullptr, &m_cbGlobalInfo, 0, 0);
    m_d3DeviceContext->VSSetConstantBuffers(static_cast<UINT>(VertexCBIndex::GlobalInfo), 1, &m_cbGlobalInfoBuff);
    m_d3DeviceContext->PSSetConstantBuffers(static_cast<UINT>(PixelCBIndex::GlobalInfo), 1, &m_cbGlobalInfoBuff);
}

void Core::DrawScene()
{
    m_cbCameraInfo.CameraPos = m_camera->GetTransform()->GetPosition();
    m_cbCameraInfo.Jitter = m_camera->GetOffset();

    m_d3DeviceContext->UpdateSubresource(m_cbCameraInfoBuff, 0, nullptr, &m_cbCameraInfo, 0, 0);
    m_d3DeviceContext->VSSetConstantBuffers(static_cast<UINT>(VertexCBIndex::CameraInfo), 1, &m_cbCameraInfoBuff);
    m_d3DeviceContext->PSSetConstantBuffers(static_cast<UINT>(PixelCBIndex::CameraInfo), 1, &m_cbCameraInfoBuff);

    m_rtvsManager->SetViewport(SizeType::Resolution);

    m_lightsManager->OnDrawingScene();

    ID3D11RenderTargetView* rtvs[2] = { m_temporaryRTV->GetRTV(), m_velocityRTV->GetRTV() };
    m_d3DeviceContext->OMSetRenderTargets(m_temporaryRTV->IsMSAA() ? 1 : 2, rtvs, m_depthStencilView);

    FLOAT bgColor[4] = { 0.34f, 0.59f, 1.0f, 1.0f };
    m_d3DeviceContext->ClearRenderTargetView(rtvs[0], bgColor);

    FLOAT vel[2] = { 0.0f,0.0f };
    m_d3DeviceContext->ClearRenderTargetView(rtvs[1], vel);

    m_d3DeviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    m_d3DeviceContext->PSSetSamplers(0, (UINT)m_samplerStates.size(), m_samplerStates.data());

    m_renderingSystem->RenderRegisteredMeshRenderers(m_camera);
}

void Core::MergeRTVsToMain()
{
    m_rtvsManager->SetViewport(SizeType::Window);

    PostProcessor::DrawPass("AdditivePP.fx", { m_temporaryRTV, m_UIRenderingSystem->GetRTV() }, m_renderTargetView);
}

void Core::OnResizeWindow(const int& width, const int& height)
{
    if (m_renderTargetView != nullptr)
        m_renderTargetView->Release();

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    m_swapChain->GetDesc(&swapChainDesc);
    HRESULT hr = m_swapChain->ResizeBuffers(swapChainDesc.BufferCount, m_window->GetWidth(), m_window->GetHeight(), swapChainDesc.BufferDesc.Format, swapChainDesc.Flags);

    ID3D11Texture2D* BackBuffer;
    hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);

    hr = m_d3Device->CreateRenderTargetView(BackBuffer, NULL, &m_renderTargetView);
    BackBuffer->Release();
}

void Core::OnChangeResolution(const int& width, const int& height)
{
    m_camera->Initialize(0.25f * 3.14f, (float)m_window->GetResolutionWidth() / m_window->GetResolutionHeight(), 0.1f, 100.0f);
    InitializeDepthStencilBuffer();
}

void Core::DestroyObject(Object* const& obj)
{
    s_instance->m_objectsToDelete.push_back(obj);
}

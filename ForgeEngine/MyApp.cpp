#include "MyApp.h"
#include "MeshRenderer.h"
#include "Object.h"
#include "Transform.h"
#include "DirectionalLight.h"
#include <DirectXCommonClasses/Time.h>
#include "Window.h"
#include "PostProcessor.h"
#include "Camera.h"
#include "RenderTargetViewsManager.h"
#include <DirectXCommonClasses/InputClass.h>
#include "AAHelpers.h"
#include "DebugLog.h"
#include "Profiler.h"
#include <DirectXTex/DirectXTex.h>
#include <functional>
#include "SSAAPerformer.h"
#include "d3d11.h"
#include "DummyAAPerformer.h"
#include "MSAAPerformer.h"
#include "FXAAPerformer.h"
#include "TAAPerformer.h"
#include "Transform.h"
#include "SSAAResolutionPerformer.h"

using namespace DirectX;
using namespace std;

void MyApp::InitScene()
{
    __super::InitScene();

    for (int x = 0; x < 4; ++x)
    {
        for (int y = 0; y < 4; ++y)
        {
            m_car = InstantiateObject<Object>();
            m_car->GetTransform()->SetPosition({ x * 10.0f, 0.0f, y * 10.0f });
            m_car->AddComponent<MeshRenderer>("model.fbx", "Base.fx");
        }
    }

    InstantiateObject<Object>()->AddComponent<DirectionalLight>(XMFLOAT3(0.5f, -1.0f, 0.5f), XMFLOAT3(1.0f, 1.0f, 1.0f), 1.0f);

    std::function<void(RTV *)> drawFunc = [this](RTV* rtv) -> void
    {
        RTV* prevRTV = m_temporaryRTV;
        m_temporaryRTV = rtv;

        Core::DrawScene();

        m_temporaryRTV = prevRTV;
    };

    m_noaa = new DummyAAPerformer(drawFunc);
    m_ssaa = new SSAAPerformer(drawFunc);
    m_ssaaRes = new SSAAResolutionPerformer(drawFunc);
    m_msaa = new MSAAPerformer(drawFunc, [this]() -> void { InitializeDepthStencilBuffer(); });
    m_fxaa = new FXAAPerformer(drawFunc);
    m_taa = new TAAPerformer(drawFunc);

    m_currentPerformer = m_noaa;
    m_currentPerformer->OnEnable();
}

void MyApp::UpdateScene()
{
    Core::UpdateScene();

    //m_car->GetTransform()->SetGlobalPosition({ 10.0f * sin(4.0f * Time::GetTime()), 0.0f, 0.0f });

    m_currentPerformer->Update();

    if (InputClass::GetKeyDown(DIK_Y))
    {
        SetAAPerformer(AAMode::None);
    }
    else if (InputClass::GetKeyDown(DIK_U))
    {
        SetAAPerformer(AAMode::SSAA);
    }
    else if (InputClass::GetKeyDown(DIK_J))
    {
        SetAAPerformer(AAMode::SSAARes);
    }
    else if (InputClass::GetKeyDown(DIK_I))
    {
        SetAAPerformer(AAMode::MSAA);
    }
    else if (InputClass::GetKeyDown(DIK_O))
    {
        SetAAPerformer(AAMode::FXAA);
    }
    else if (InputClass::GetKeyDown(DIK_P))
    {
        SetAAPerformer(AAMode::TAA);
    }

    if (InputClass::GetKeyDown(DIK_M))
        MakeScreenshot();

    if (InputClass::GetKeyDown(DIK_X))
    {
        Profiler::Reset();
        DebugLog::Log("Reset profiler", 1.0f);
    }
}

void MyApp::PostProcessing()
{
    m_currentPerformer->PostProcessing();

    GetD3DeviceContext()->CopyResource(m_temporaryRTV->GetTexture(), m_currentPerformer->GetOutputRTV()->GetTexture());
}

void MyApp::DrawScene()
{
    m_currentPerformer->DrawScene();
}

void MyApp::FillDepthStencilDescWithDefaultValues(D3D11_TEXTURE2D_DESC& desc)
{
    Core::FillDepthStencilDescWithDefaultValues(desc);
    
    if (m_msaa && m_msaa == m_currentPerformer)
        m_msaa->FillDepthStencilDescWithDefaultValues(desc);
}

void MyApp::OnResizeWindow(const int& width, const int& height)
{
    Core::OnResizeWindow(width, height);
    //GetWindow()->SetResolution(GetWindow()->GetWidth() * 8, GetWindow()->GetHeight() * 8);
}

void MyApp::SetAAPerformer(AAMode mode)
{
    m_currentPerformer->OnDisable();

    if (mode == AAMode::None)
    {
        m_currentPerformer = m_noaa;
    }
    else if (mode == AAMode::SSAA)
    {
        m_currentPerformer = m_ssaa;
    }
    else if (mode == AAMode::SSAARes)
    {
        m_currentPerformer = m_ssaaRes;
    }
    else if (mode == AAMode::MSAA)
    {
        m_currentPerformer = m_msaa;
    }
    else if (mode == AAMode::FXAA)
    {
        m_currentPerformer = m_fxaa;
    }
    else if (mode == AAMode::TAA)
    {
        m_currentPerformer = m_taa;
    }

    m_currentPerformer->OnEnable();
}

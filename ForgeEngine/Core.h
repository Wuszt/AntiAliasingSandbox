#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <vector>
#include <unordered_set>
#include <type_traits>
#include "ConstantBuffers.h"

class Window;
class Camera;
class Object;
class RenderingSystem;
class ShadersManager;
class RenderTargetViewsManager;
class PostProcessor;
class RTV;
class LightsManager;
class UIRenderingSystem;

class Core
{
public:
    Core();
    virtual ~Core();

    void Run(const HINSTANCE& hInstance, const int& ShowWnd, const int& width, const int& height, int resW, int resH, std::string resultsPath);

    static inline RenderingSystem* GetRenderingSystem() { return s_instance->m_renderingSystem; }
    static inline UIRenderingSystem* GetUIRenderingSystem() { return s_instance->m_UIRenderingSystem; }
    static inline LightsManager* GetLightsManager() { return s_instance->m_lightsManager; }
    static inline ID3D11Device* GetD3Device() { return s_instance->m_d3Device; }
    static inline ID3D11DeviceContext* GetD3DeviceContext() { return s_instance->m_d3DeviceContext; }
    static inline RenderTargetViewsManager* GetRTVsManager() { return s_instance->m_rtvsManager; }
    static inline Window* GetWindow() { return s_instance->m_window; }
    static inline Camera* GetCamera() { return s_instance->m_camera; }
    static inline RTV* GetVelocityBuffer() { return s_instance->m_velocityRTV; }
    static inline RTV* GetDepthStencilBuffer() { return s_instance->m_depthStencilRTV; }
    static void MakeScreenshot(std::string name = "");
    static void RequestScreenshot(std::string name = "");
    static inline std::string GetResultsPath() { return s_instance->m_resultsPath; }

    template<typename T, typename ... Args>
    static T* InstantiateObject(Args&&... args)
    {
        static_assert(std::is_base_of<Object, T>::value, "T must be an object!");

        T* obj = new T(std::forward<Args>(args)...);

        s_instance->m_objectsToAdd.push_back(obj);

        return obj;
    }

    static void OnResizeCallback(const int& w, const int& h) //too fix in future
    {
        s_instance->OnResizeWindow(w, h);
    }

    static void OnChangeResolutionCallback(const int& w, const int& h) //too fix in future
    {
        s_instance->OnChangeResolution(w, h);
    }

protected:
    virtual void Initialize(const HINSTANCE& hInstance, const int& ShowWnd, const int& width, const int& height);

    virtual HRESULT InitializeD3D();
    virtual HRESULT InitializeSwapChain();
    virtual HRESULT InitializeDepthStencilBuffer();

    virtual void InitScene();

    virtual void UpdateScene();

    virtual void MainRTVProcessing();
    virtual void DrawScene();
    virtual void PostProcessing();

    virtual void FillDepthStencilDescWithDefaultValues(D3D11_TEXTURE2D_DESC& desc);
    virtual void FillSwapChainBufferDescWithDefaultValues(DXGI_MODE_DESC& desc);
    virtual void FillSwapChainDescWithDefaultValues(DXGI_SWAP_CHAIN_DESC& desc);

    virtual RTV* GetRTVForTemporary();
        
    virtual void OnResizeWindow(const int& width, const int& height);
    virtual void OnChangeResolution(const int& width, const int& height);

    Camera* m_camera;
    RTV* m_temporaryRTV;
    RTV* m_velocityRTV;

private:
    void AddPendingObjects();
    void DeletePendingObjects();

    void BeforeUpdateScene();
    void AfterUpdateScene();
    void MergeRTVsToMain();

    static void DestroyObject(Object* const& obj);

    static Core* s_instance;

    std::unordered_set<Object*> m_objects;
    std::vector<Object*> m_objectsToAdd;
    std::vector<Object*> m_objectsToDelete;

    IDXGISwapChain* m_swapChain;
    ID3D11Device* m_d3Device;
    ID3D11DeviceContext* m_d3DeviceContext;
    ID3D11RenderTargetView* m_renderTargetView;
    ID3D11DepthStencilView* m_depthStencilView;
    RTV* m_depthStencilRTV;
    ID3D11Texture2D* m_depthStencilBuffer;

    UIRenderingSystem* m_UIRenderingSystem;
    RenderingSystem* m_renderingSystem;
    Window* m_window;
    RenderTargetViewsManager* m_rtvsManager;
    LightsManager* m_lightsManager;

    cbPerFrame m_cbPerFrame;
    ID3D11Buffer* m_cbPerFrameBuff;

    cbGlobalInfo m_cbGlobalInfo;
    ID3D11Buffer* m_cbGlobalInfoBuff;

    cbCameraInfo m_cbCameraInfo;
    ID3D11Buffer* m_cbCameraInfoBuff;

    ID3D11RasterizerState* m_rasterizerState;

    std::string m_resultsPath;

    std::string m_requestedSSFileName;
    bool m_isSSRequested = false;

    //to move
    std::vector<ID3D11SamplerState*> m_samplerStates;
    //end to move
};


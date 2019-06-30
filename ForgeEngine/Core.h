#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <vector>
#include <unordered_set>
#include <type_traits>

class Window;
class Camera;
class Object;
class RenderingSystem;
class ShadersManager;

class Core
{
public:
    Core();
    virtual ~Core();

    void Run(const HINSTANCE& hInstance, const int& ShowWnd, const int& width, const int& height);

    static inline RenderingSystem* GetRenderingSystem() { return s_instance->m_renderingSystem; }

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

protected:
    virtual void Initialize(const HINSTANCE& hInstance, const int& ShowWnd, const int& width, const int& height);

    virtual HRESULT InitializeD3D();
    virtual HRESULT InitializeSwapChain();
    virtual HRESULT InitializeDepthStencilBuffer();
    virtual void InitializeViewport();

    virtual void InitScene();

private:
    void FillDepthStencilDescWithDefaultValues(D3D11_TEXTURE2D_DESC& desc);
    void FillSwapChainBufferDescWithDefaultValues(DXGI_MODE_DESC& desc);
    void FillSwapChainDescWithDefaultValues(DXGI_SWAP_CHAIN_DESC& desc);

    void AddPendingObjects();
    void DeletePendingObjects();

    void BeforeUpdateScene();
    void UpdateScene();
    void AfterUpdateScene();
    void DrawScene();

    void OnResizeWindow(const int& width, const int& height);

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
    ID3D11Texture2D* m_depthStencilBuffer;

    RenderingSystem* m_renderingSystem;
    Window* m_window;
    Camera* m_camera;

    ID3D11Query* queryStart;
    ID3D11Query* queryEnd;
    ID3D11Query* disjoint0;

    //to move
    ID3D11SamplerState* samplerState;
    //end to move
};


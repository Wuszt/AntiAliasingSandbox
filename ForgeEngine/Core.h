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

class Core
{
public:
    Core(const HINSTANCE& hInstance, const int& ShowWnd, const int& width, const int& height);
    virtual ~Core();

    void Run();

    static inline RenderingSystem* GetRenderingSystem() { return s_instance->m_renderingSystem; }

    template<typename T, typename ... Args>
    static T* InstantiateObject(Args&&... args)
    {
        static_assert(std::is_base_of<Object, T>::value, "T must be an object!");

        T* obj = new T(std::forward<Args>(args)...);

        s_instance->m_objectsToAdd.push_back(obj);

        return obj;
    }

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

    void AddPendingObjects();
    void DeletePendingObjects();

    bool InitScene();
    void BeforeUpdateScene();
    void UpdateScene();
    void AfterUpdateScene();
    void DrawScene();

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

    Object* m_obj0;

    //to move
    ID3D11VertexShader* VS;
    ID3D11PixelShader* PS;
    ID3D10Blob* VS_Buffer;
    ID3D10Blob* PS_Buffer;
    ID3D11InputLayout* vertLayout;

    ID3D11SamplerState* samplerState;

    D3D11_INPUT_ELEMENT_DESC layout[2] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);

    //end to move
};


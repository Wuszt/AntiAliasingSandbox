#pragma once
#include <unordered_set>

struct ID3D11Device;
struct ID3D11RenderTargetView;
struct ID3D11DeviceContext;
class Window;

class RTV
{
public:
    ID3D11RenderTargetView* GetRTV() const;
    ID3D11Texture2D* GetTexture() const;

private:
    RTV(ID3D11RenderTargetView* const& rtv, ID3D11Texture2D* const& tex);
    ~RTV();

    void Reinitialize(ID3D11RenderTargetView* const& rtv, ID3D11Texture2D* const& tex);
    void Release();

    friend class RenderTargetViewsManager;

    ID3D11RenderTargetView* m_rtv;
    ID3D11Texture2D* m_texture;
};

class RenderTargetViewsManager
{
public:
    RenderTargetViewsManager(ID3D11Device* const& device, ID3D11DeviceContext* const& context, Window* const& window);
    ~RenderTargetViewsManager();

    RTV* AcquireRTV();
    void ReleaseRTV(RTV* const& rtv);

    static void OnWindowResize(const int& width, const int& height);

    static RenderTargetViewsManager* s_instance;

private:
    void CreateRTVComponents(ID3D11Texture2D*& tex, ID3D11RenderTargetView*& rtv);
    RTV* GetOrCreateRTV();
    void ResizeRTVs();

    ID3D11Device* m_d3Device;
    ID3D11DeviceContext* m_d3Context;
    Window* m_window;

    std::unordered_set<RTV*> m_availableRTVs;
    std::unordered_set<RTV*> m_acquiredRTVs;
};


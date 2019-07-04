#pragma once
#include <unordered_set>

struct ID3D11Device;
struct ID3D11RenderTargetView;
class Window;

class RTV
{
public:
    RTV(ID3D11RenderTargetView* const& rtv, ID3D11Texture2D* const& tex);
    ID3D11RenderTargetView* GetRTV();
    ID3D11Texture2D* GetTexture();

private:
    void Initialize(ID3D11RenderTargetView* const& rtv, ID3D11Texture2D* const& tex);

    ID3D11RenderTargetView* m_rtv;
    ID3D11Texture2D* m_texture;
};

class RenderTargetViewsManager
{
public:
    RenderTargetViewsManager(ID3D11Device* const& device, Window* const& window);
    ~RenderTargetViewsManager();

    RTV* AcquireRTV();
    void ReleaseRTV(RTV* const& rtv);

    static void OnWindowResize(const int& width, const int& height);

    static RenderTargetViewsManager* s_instance;

private:
    RTV* CreateRTV();
    RTV* GetOrCreateRTV();

    ID3D11Device* m_d3Device;
    Window* m_window;

    std::unordered_set<RTV*> m_availableRTVs;
    std::unordered_set<RTV*> m_acquiredRTVs;
};


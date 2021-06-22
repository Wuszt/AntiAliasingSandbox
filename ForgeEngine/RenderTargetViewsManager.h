#pragma once
#include <unordered_set>
#include <unordered_map>

struct ID3D11RenderTargetView;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;

class Window;

enum class SizeType
{
    Window,
    Resolution
};

class RTV
{
public:
    ID3D11RenderTargetView* GetRTV() const;
    ID3D11Texture2D* GetTexture() const;
    ID3D11ShaderResourceView* GetSRV() const;

    bool IsNotPooled() const { return IsMSAA() || m_format != DXGI_FORMAT_B8G8R8A8_UNORM; }
    bool IsMSAA() const { return m_samplesAmount > 1; }

    RTV(ID3D11RenderTargetView* const& rtv, ID3D11Texture2D* const& tex, ID3D11ShaderResourceView* const& srv, int samplesAmount, DXGI_FORMAT format);
    ~RTV();

private:
    void Reinitialize(ID3D11RenderTargetView* const& rtv, ID3D11Texture2D* const& tex, ID3D11ShaderResourceView* const& srv);
    void Release();

    friend class RenderTargetViewsManager;

    ID3D11RenderTargetView* m_rtv;
    ID3D11Texture2D* m_texture;
    ID3D11ShaderResourceView* m_srv;
    const int m_samplesAmount;
    const DXGI_FORMAT m_format;
};

class RenderTargetViewsManager
{
public:
    RenderTargetViewsManager(Window* const& window);
    ~RenderTargetViewsManager();

    RTV* AcquireRTV(const SizeType& sizeType, const int textureSamplesAmount = 1, DXGI_FORMAT format = DXGI_FORMAT_B8G8R8A8_UNORM);
    void ReleaseRTV(RTV* const& rtv);
    void SetViewport(SizeType size);

    static void OnWindowResize(const int& width, const int& height);
    static void OnResolutionChange(const int& width, const int& height);

    static RenderTargetViewsManager* s_instance;

private:
    void CreateRTVComponents(ID3D11Texture2D*& tex, ID3D11RenderTargetView*& rtv, ID3D11ShaderResourceView*& srv, const SizeType& sizeType, int textureSamplesAmount, DXGI_FORMAT format);

    RTV* GetOrCreatePooledRTV(const SizeType& sizeType);
    RTV* CreateNewRTV(const SizeType& sizeType, int textureSamplesAmount, DXGI_FORMAT format);
    void ResizeRTVs(const SizeType& sizeType);

    void InitializeViewport(SizeType size);

    Window* m_window;

    std::unordered_map<SizeType, std::unordered_set<RTV*>> m_availableRTVs;
    std::unordered_map<RTV*, SizeType> m_acquiredRTVs;

    std::unordered_map<SizeType, D3D11_VIEWPORT> m_viewPorts;

    SizeType m_currentViewportSizeType;
};


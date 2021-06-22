#pragma once
#include <string>

class RTV;

struct IFW1Factory;
struct IFW1FontWrapper;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;

namespace DirectX
{
    struct XMFLOAT4;
}
enum class TextAnchor
{
    Top = 0,
    Center = 1,
    Bottom = 2,
    Left = 4,
    Right = 8
};

inline TextAnchor operator| (const TextAnchor& l, const TextAnchor& r) { return static_cast<TextAnchor>(static_cast<unsigned int>(l) | static_cast<unsigned int>(r)); }

class UIRenderingSystem
{
public:
    UIRenderingSystem();
    ~UIRenderingSystem();

    void OnBeforeDrawing();
    void Print(const std::string& text, const float& size, const float& x, const float& y, const DirectX::XMFLOAT4& color, const TextAnchor& anchor = TextAnchor::Center) const;
    RTV* GetRTV() const;

private:
    IFW1Factory* m_textFactory;
    IFW1FontWrapper* m_fontWrapper;
    RTV* m_rtv;
};


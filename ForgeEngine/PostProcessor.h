#pragma once
#include <string>
#include <vector>

class ShadersManager;
struct ID3D11Texture2D;
struct ID3D11RenderTargetView;
struct ID3D11InputLayout;
class RTV;

class PostProcessor
{
public:
    PostProcessor() = delete;
    ~PostProcessor() = delete;

    static void Initialize();
    static void Release();

    static void DrawPass(const std::string& shaderName, const std::vector<RTV*>& input, ID3D11RenderTargetView* const& target);
    static void DrawPass(const std::string& shaderName, const std::vector<RTV*>& input, RTV* const& target);

private:
    static ID3D11InputLayout* s_inputLayout;
};


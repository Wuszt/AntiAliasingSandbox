#pragma once
#include <string>
#include <vector>

class ShadersManager;
struct ID3D11Texture2D;
struct ID3D11RenderTargetView;
struct ID3D11InputLayout;

class PostProcessor
{
public:
    PostProcessor() = delete;
    ~PostProcessor() = delete;

    static void Initialize();
    static void Release();

    static void DrawPass(const std::string& shaderName, const std::vector<ID3D11Texture2D*>& textures, ID3D11RenderTargetView* const& target);

private:
    static ID3D11InputLayout* s_inputLayout;
};


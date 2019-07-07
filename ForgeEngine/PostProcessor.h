#pragma once
#include <string>
#include <vector>

class ShadersManager;
struct ID3D11Texture2D;
struct ID3D11RenderTargetView;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11InputLayout;

class PostProcessor
{
public:
    PostProcessor(ID3D11Device* const& device, ID3D11DeviceContext* const& context);
    ~PostProcessor();

    void DrawPass(const std::string& shaderName, const std::vector<ID3D11Texture2D*>& textures, ID3D11RenderTargetView* const& target);

    static void DrawPass(ID3D11Device* const& device, ID3D11DeviceContext* const& context, const std::string& shaderName, const std::vector<ID3D11Texture2D*>& textures, ID3D11RenderTargetView* const& target);

private:
    ID3D11Device* m_d3Device;
    ID3D11DeviceContext* m_d3Context;
    static ID3D11InputLayout* s_inputLayout;
};


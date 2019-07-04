#pragma once
#include <string>
#include <vector>

class ShadersManager;
struct ID3D11Texture2D;
struct ID3D11RenderTargetView;
struct ID3D11Device;
struct ID3D11DeviceContext;

class PostProcessor
{
public:
    PostProcessor(ID3D11Device* const& device, ID3D11DeviceContext* const& context, ShadersManager* const& shadersManager);
    ~PostProcessor();

    void DrawPass(const std::string& shaderName, const std::vector<ID3D11Texture2D*>& textures, ID3D11RenderTargetView* const& target);

private:
    ShadersManager* m_shadersManager;
    ID3D11Device* m_d3Device;
    ID3D11DeviceContext* m_d3Context;
};


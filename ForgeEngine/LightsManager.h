#pragma once
#include <DirectXMath.h>
#include <vector>

struct ID3D11Buffer;
class Light;
class DirectionalLight;


class LightsManager
{
public:
    LightsManager();
    ~LightsManager();

    void OnDrawingScene();
    void AddLight(Light* const& light);

    inline void SetAmbient(const DirectX::XMFLOAT3& ambient) { m_ambient = ambient; }

private:
    DirectX::XMFLOAT3 m_ambient = DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f);
    ID3D11Buffer* m_buffer;
    std::vector<DirectionalLight*> m_directionalLights;
};


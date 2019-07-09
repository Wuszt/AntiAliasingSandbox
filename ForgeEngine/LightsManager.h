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

    ID3D11Buffer* m_buffer;
    std::vector<DirectionalLight*> m_directionalLights;
};


#pragma once
#include "Light.h"
#include <DirectXMath.h>

struct cbDirectionalLight
{
    DirectX::XMFLOAT3 Direction;
    float pad0;
    DirectX::XMFLOAT3 Color;
    float pad1;
};

class DirectionalLight : public Light
{
public:
    DirectionalLight(Object* const& owner, const DirectX::XMFLOAT3& dir,const DirectX::XMFLOAT3& color, const float& intensity);

    cbDirectionalLight& GetData();
private:
    cbDirectionalLight m_currentData;
};


#pragma once
#include <DirectXMath.h>

#include "Object.h"

class Camera : public Object
{
public:
    Camera(const float& fov, const float& aspectRatio, const float& nearClip, const float& farClip);
    virtual ~Camera();

    DirectX::XMMATRIX GetViewMatrix();
    inline DirectX::XMMATRIX GetProjectionMatrix() { return m_projectionMatrix; }

private:
    DirectX::XMMATRIX m_projectionMatrix;
};


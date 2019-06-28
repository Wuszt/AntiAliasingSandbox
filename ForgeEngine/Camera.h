#pragma once
#include <DirectXMath.h>

#include "Object.h"

class Camera : public Object
{
public:
    Camera();
    virtual ~Camera();

    DirectX::XMMATRIX GetViewMatrix();
    inline DirectX::XMMATRIX GetProjectionMatrix() const { return m_projectionMatrix; }
    void Initialize(const float& fov, const float& aspectRatio, const float& nearClip, const float& farClip);

private:
    DirectX::XMMATRIX m_projectionMatrix;
};


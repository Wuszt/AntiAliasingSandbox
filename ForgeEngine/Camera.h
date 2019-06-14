#pragma once
#include <DirectXMath.h>

#include "Object.h"

class Camera : public Object
{
public:
    Camera(const float& fov, const float& aspectRatio, const float& nearClip, const float& farClip);
    virtual ~Camera();

    void SetCamPos(const float& x, const float& y, const float& z);
    void SetCamPos(const DirectX::XMFLOAT3& pos);

    void LookAt(const float& x, const float& y, const float& z);
    void LookAt(const DirectX::XMFLOAT3& target);

    DirectX::XMMATRIX GetViewMatrix();
    inline DirectX::XMMATRIX GetProjectionMatrix() { return m_projectionMatrix; }

private:
    DirectX::XMMATRIX m_projectionMatrix;
};


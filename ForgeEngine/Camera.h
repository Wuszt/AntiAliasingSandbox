#pragma once
#include <DirectXMath.h>

#include "SceneObject.h"

class Camera : public SceneObject
{
public:
    Camera(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& lookAt, const float& fov, const float& aspectRatio, const float& nearClip, const float& farClip);
    virtual ~Camera();

    void SetCamPos(float x, float y, float z);
    void SetLookAt(float x, float y, float z);

private:
    DirectX::XMMATRIX m_projectionMatrix;

public:
    DirectX::XMMATRIX GetViewMatrix();
    inline DirectX::XMMATRIX GetProjectionMatrix() { return m_projectionMatrix; }
};


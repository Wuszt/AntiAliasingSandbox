#pragma once
#include "Camera.h"
class ControllableCamera : public Camera
{
public:
    ControllableCamera(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& lookAt, const float& fov, const float& aspectRatio, const float& nearClip, const float& farClip)
        : Camera(pos, lookAt, fov, aspectRatio, nearClip, farClip) {}
    ~ControllableCamera();
    virtual void Update() override;
};


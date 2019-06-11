#pragma once
#include "Camera.h"
class ControllableCamera : public Camera
{
public:
    ControllableCamera(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rotation, const float& fov, const float& aspectRatio, const float& nearClip, const float& farClip) : Camera(pos, rotation, fov, aspectRatio, nearClip, farClip) {}
    ~ControllableCamera();
    virtual void Update() override;
};


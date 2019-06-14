#pragma once
#include "Camera.h"
class ControllableCamera : public Camera
{
public:
    ControllableCamera(const float& fov, const float& aspectRatio, const float& nearClip, const float& farClip) : Camera(fov, aspectRatio, nearClip, farClip) {}
    ~ControllableCamera();
    virtual void Update() override;
};


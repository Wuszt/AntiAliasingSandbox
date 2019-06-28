#pragma once
#include "Camera.h"
class ControllableCamera : public Camera
{
public:
    ControllableCamera() {}
    ~ControllableCamera();
    virtual void Update() override;
};


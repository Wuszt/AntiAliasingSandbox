#include "Camera.h"
#include "Transform.h"

using namespace DirectX;

Camera::Camera() : Object()
{
    Name = "Camera";
}

Camera::~Camera()
{
}

DirectX::XMMATRIX Camera::GetViewMatrix()
{
    return XMMatrixInverse(nullptr, m_transform->GetWorldMatrix());
}

void Camera::Initialize(const float& fov, const float& aspectRatio, const float& nearClip, const float& farClip)
{
    m_projectionMatrix = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearClip, farClip);
}

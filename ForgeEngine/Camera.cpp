#include "Camera.h"
#include "Transform.h"

using namespace DirectX;

Camera::Camera(const float& fov, const float& aspectRatio, const float& nearClip, const float& farClip) : Object()
{
    m_projectionMatrix = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearClip, farClip);
}

Camera::~Camera()
{
}

DirectX::XMMATRIX Camera::GetViewMatrix()
{
    return XMMatrixInverse(nullptr, m_transform->GetWorldMatrix());
}

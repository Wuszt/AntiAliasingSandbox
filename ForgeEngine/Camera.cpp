#include "Camera.h"
#include "Transform.h"

using namespace DirectX;

Camera::Camera(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rotation, const float& fov, const float& aspectRatio, const float& nearClip, const float& farClip) : Object(pos)
{
    SetCamPos(pos);
    m_transform->SetRotation(rotation.x, rotation.y, rotation.z);
    m_projectionMatrix = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearClip, farClip);
}

Camera::~Camera()
{
}

void Camera::SetCamPos(const float& x, const float& y, const float& z)
{
    m_transform->SetPosition(x, y, z);
}

void Camera::SetCamPos(const DirectX::XMFLOAT3& pos)
{
    m_transform->SetPosition(pos);
}

void Camera::LookAt(const float& x, const float& y, const float& z)
{
    m_transform->LookAt(XMFLOAT3(x,y,z));
}

void Camera::LookAt(const XMFLOAT3& target)
{
    m_transform->LookAt(target);
}

DirectX::XMMATRIX Camera::GetViewMatrix()
{
    return XMMatrixInverse(nullptr, m_transform->GetWorldMatrix());
}

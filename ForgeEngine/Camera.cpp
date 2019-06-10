#include "Camera.h"
#include "Transform.h"

using namespace DirectX;

Camera::Camera(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& lookAt, const float& fov, const float& aspectRatio, const float& nearClip, const float& farClip) : SceneObject(pos)
{
    //m_camPos = XMLoadFloat3(&pos);
    //m_camLookAt = XMLoadFloat3(&lookAt);
    //m_camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    m_projectionMatrix = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearClip, farClip);
}

Camera::~Camera()
{
}

void Camera::SetCamPos(const float& x, const float& y, const float& z)
{
    m_transform->SetPosition(x, y, z);
}

void Camera::LookAt(const float& x, const float& y, const float& z)
{
    m_transform->LookAt(XMFLOAT3(x,y,z));
}

DirectX::XMMATRIX Camera::GetViewMatrix()
{
    return XMMatrixInverse(nullptr, m_transform->GetWorldMatrix());
}

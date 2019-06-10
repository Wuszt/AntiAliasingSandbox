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

void Camera::SetLookAt(float x, float y, float z)
{
    //m_isViewDirty = true;
    //m_camLookAt = XMVectorSet(x, y, z, 0.0f);
}

DirectX::XMMATRIX Camera::GetViewMatrix()
{
    return XMMatrixInverse(nullptr, m_transform->GetWorldMatrix());
}

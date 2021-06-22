#include "Camera.h"
#include "Transform.h"
#include "DebugLog.h"
#include <DirectXCommonClasses/InputClass.h>

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
    SetOffset(m_offset);
}

void Camera::Update()
{
    DebugLog::Log("Camera position: " + to_string(m_transform->GetPosition()));
    DebugLog::Log("Camera rotation: " + to_string(m_transform->GetRotationAsEulerInDegrees()));

    for (int i = 0; i < 10; ++i)
    {
        if (InputClass::GetKeyDown(DIK_1 + i))
        {
            SetSavedPosition(i + 1);
        }
    }
}

void Camera::SetOffset(const DirectX::XMFLOAT2& offset)
{
    m_offset = offset;
    DirectX::XMMATRIX translation = XMMatrixTranslation(offset.x, offset.y, 0.0f);

    m_projectionMatrixWithOffset = m_projectionMatrix * translation;
}

void Camera::SetSavedPosition(int i)
{
    if (i == 1)
    {
        m_transform->SetPosition({ 5.35631f, 4.27908f, -5.81734f });
        m_transform->SetGlobalRotationFromEulerDegrees({ 18.1628f, -42.0552f, 0.0f });
    }
    else if (i == 2)
    {
        m_transform->SetPosition({ -0.594312f, 1.51018f, -2.20235f });
        m_transform->SetGlobalRotationFromEulerDegrees({ -2.86476f, -83.1937f, 0.0f });
    }
    else if (i == 3)
    {
        m_transform->SetPosition({ -5.8824f, 5.5984f, -6.54079f });
        m_transform->SetGlobalRotationFromEulerDegrees({ 22.6319f, 51.0503f, 0.0f });
    }
    else if (i == 4)
    {
        m_transform->SetPosition({ -0.709111f, 3.02254f, 8.95479f });
        m_transform->SetGlobalRotationFromEulerDegrees({ 6.7611f, 53.4564f, 0.0f });
    }
    else if (i == 5)
    {
        m_transform->SetPosition({ 0.559315f, 0.565559f, -2.65601f });
        m_transform->SetGlobalRotationFromEulerDegrees({ 1.08901f, 1.08901f, 0.0f });
    }
}

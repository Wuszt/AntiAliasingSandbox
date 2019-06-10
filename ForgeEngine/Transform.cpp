#include "Transform.h"

using namespace DirectX;

Transform::Transform(const XMFLOAT3& pos)
{
    SetPosition(pos);
}

Transform::~Transform()
{
}

XMMATRIX Transform::GetWorldMatrix()
{
    if (m_isDirty)
        m_worldMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&m_rotation)) * XMMatrixTranslationFromVector(XMLoadFloat3(&m_position));

    m_isDirty = false;
    return m_worldMatrix;
}

void Transform::SetPosition(const XMFLOAT3& pos)
{
    m_position = pos;
    m_isDirty = true;
}

void Transform::SetPosition(const float& x, const float& y, const float& z)
{
    m_position = XMFLOAT3(x, y, z);
    m_isDirty = true;
}

void Transform::SetRotation(const float& x, const float& y, const float& z)
{
    XMStoreFloat4(&m_rotation, XMQuaternionRotationRollPitchYaw(x, y, z));
    m_isDirty = true;
}

void Transform::SetRotation(const XMFLOAT4& quaternion)
{
    m_rotation = quaternion;
    m_isDirty = true;
}

//JUST PROTO
void Transform::LookAt(const XMFLOAT3& target)
{
    XMVECTOR forward = XMVector3Normalize(XMLoadFloat3(&target) - XMLoadFloat3(&m_position));

    XMVECTOR globalForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

    XMVECTOR rotationAxis = XMVector3Cross(forward, globalForward);

    rotationAxis = XMVector3Normalize(rotationAxis);

    XMVECTOR vecDot = XMVector3Dot(forward, globalForward);

    float angle = acosf(XMVectorGetX(vecDot));
    
    if (angle < 0.001f )
        return;
    
    angle = -angle;

    XMFLOAT3 tmp;
    XMStoreFloat3(&tmp, rotationAxis);

    XMVECTOR quaternion = XMQuaternionRotationAxis(rotationAxis, angle);

    XMFLOAT4 rotation;
    XMStoreFloat4(&rotation, quaternion);

    SetRotation(rotation);
}

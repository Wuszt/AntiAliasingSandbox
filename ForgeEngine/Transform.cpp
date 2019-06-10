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
        m_worldMatrix = XMMatrixRotationQuaternion(m_rotation) * XMMatrixTranslationFromVector(m_position);

    m_isDirty = false;
    return m_worldMatrix;
}

void Transform::SetPosition(const XMFLOAT3& pos)
{
    m_position = XMLoadFloat3(&pos);
    m_isDirty = true;
}

void Transform::SetPosition(const float& x, const float& y, const float& z)
{
    m_position = XMVectorSet(x, y, z, 1.0f);
    m_isDirty = true;
}

void Transform::SetRotation(const float& x, const float& y, const float& z)
{
    m_rotation = XMQuaternionRotationRollPitchYaw(x, y, z);
    m_isDirty = true;
}

#include "Transform.h"

using namespace DirectX;

Transform::Transform(const XMFLOAT3& pos)
{
    SetPosition(pos);
    SetRotationFromEuler(XMFLOAT3(0.0f, 0.0f, 0.0f));
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

void Transform::Translate(const float& x, const float& y, const float& z)
{
    XMVECTOR dir = XMVectorSet(x, y, z, 0.0f);
    XMVECTOR rotation = XMLoadFloat4(&m_rotation);
    dir = XMVector3Rotate(dir, rotation);

    TranslateInWorld(XMVectorGetX(dir), XMVectorGetY(dir), XMVectorGetZ(dir));
}

void Transform::TranslateInWorld(const float& x, const float& y, const float& z)
{
    m_position.x += x;
    m_position.y += y;
    m_position.z += z;

    m_isDirty = true;
}

DirectX::XMFLOAT3 Transform::GetRotationAsEuler() const
{
    XMFLOAT4 quat = GetRotation();

    float yaw;
    float pitch;
    float roll;
    float test = quat.x*quat.y + quat.z*quat.w;

    if (test > 0.499) {
        yaw = (2 * atan2f(quat.x, quat.w));
        pitch = static_cast<float>(XM_PI / 2.0f);
        roll = 0.0f;
        return XMFLOAT3(pitch, yaw, roll);
    }
    if (test < -0.499) {
        yaw = (-2.0f * atan2f(quat.x, quat.w));
        pitch = static_cast<float>(-XM_PI / 2.0f);
        roll = 0.0f;
        return XMFLOAT3(pitch, yaw, roll);
    }

    float sqx = quat.x*quat.x;
    float sqy = quat.y*quat.y;
    float sqz = quat.z*quat.z;
    yaw = (atan2f(2 * quat.y*quat.w - 2 * quat.x*quat.z, 1 - 2 * sqy - 2 * sqz));
    pitch = (asinf(2 * test));
    roll = (atan2f(2 * quat.x*quat.w - 2 * quat.y*quat.z, 1 - 2 * sqx - 2 * sqz));

    return XMFLOAT3(pitch, yaw, roll);
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
void Transform::RotateLocal(const float& x, const float& y, const float& z)
{
    XMVECTOR curr = XMLoadFloat4(&m_rotation);
    curr = XMQuaternionNormalize(curr);

    XMVECTOR toRotate = XMQuaternionRotationRollPitchYaw(x, y, z);
    toRotate = XMQuaternionNormalize(toRotate);

    curr = XMQuaternionMultiply(toRotate, curr);

    XMFLOAT4 fRotation;
    XMStoreFloat4(&fRotation, curr);

    SetRotation(fRotation);
}

void Transform::RotateGlobal(const float& x, const float& y, const float& z)
{
    XMVECTOR curr = XMLoadFloat4(&m_rotation);
    curr = XMQuaternionNormalize(curr);

    XMVECTOR toRotate = XMQuaternionRotationRollPitchYaw(x, y, z);
    toRotate = XMQuaternionNormalize(toRotate);

    curr = XMQuaternionMultiply(curr, toRotate);

    XMFLOAT4 fRotation;
    XMStoreFloat4(&fRotation, curr);

    SetRotation(fRotation);
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

    if (angle < 0.001f)
        return;

    angle = -angle;

    XMFLOAT3 tmp;
    XMStoreFloat3(&tmp, rotationAxis);

    XMVECTOR quaternion = XMQuaternionRotationAxis(rotationAxis, angle);

    XMFLOAT4 rotation;
    XMStoreFloat4(&rotation, quaternion);

    SetRotation(rotation);
}

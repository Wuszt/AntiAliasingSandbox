#include "Transform.h"
#include "Object.h"

using namespace DirectX;

Transform::Transform(Object* owner) : Component(owner)
{
    SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
    SetRotationFromEulerDegrees(XMFLOAT3(0.0f, 0.0f, 0.0f));
    SetScale({ 1.0f, 1.0f, 1.0f });
}

Transform::~Transform()
{
}

XMMATRIX Transform::GetWorldMatrix()
{
    if (!m_isDirty)
        return m_worldMatrix;

    if (m_parent != nullptr)
        m_worldMatrix = m_parent->GetWorldMatrix();
    else
        m_worldMatrix = XMMatrixIdentity();

    m_worldMatrix = XMMatrixScalingFromVector(XMLoadFloat3(&m_scale)) * XMMatrixRotationQuaternion(XMLoadFloat4(&m_rotation)) * XMMatrixTranslationFromVector(XMLoadFloat3(&m_position)) * m_worldMatrix;

    m_isDirty = false;
    return m_worldMatrix;
}

void Transform::SetScale(const DirectX::XMFLOAT3& scale)
{
    m_scale = scale;

    SetDirty();
}

void Transform::SetGlobalScale(DirectX::XMFLOAT3 scale)
{
    XMVECTOR vecScale = XMLoadFloat3(&scale);
    XMVECTOR vecGlobalScale = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);

    if (m_parent != nullptr)
    {
        XMVECTOR rot;
        XMVECTOR translation;
        XMMatrixDecompose(&vecGlobalScale, &rot, &translation, m_parent->GetWorldMatrix());
    }

    vecScale = XMVectorDivide(vecScale, vecGlobalScale);

    XMStoreFloat3(&scale, vecScale);

    SetScale(scale);
}

void Transform::SetPosition(const XMFLOAT3& pos)
{
    m_position = pos;
    SetDirty();
}

void Transform::SetGlobalPosition(DirectX::XMFLOAT3 pos)
{
    XMMATRIX matrix = XMMatrixIdentity();

    if (m_parent != nullptr)
    {
        matrix = XMMatrixInverse(nullptr, m_parent->GetWorldMatrix());
    }

    XMVECTOR vecPos = XMLoadFloat3(&pos);
    vecPos = XMVector3Transform(vecPos, matrix);
    XMStoreFloat3(&pos, vecPos);

    SetPosition(pos);
}

void Transform::Translate(const DirectX::XMFLOAT3& offset)
{
    XMVECTOR dir = XMLoadFloat3(&offset);
    XMVECTOR rotation = XMLoadFloat4(&m_rotation);
    dir = XMVector3Rotate(dir, rotation);

    XMFLOAT3 vec;
    XMStoreFloat3(&vec, dir);

    TranslateInWorld(vec);
}

void Transform::TranslateInWorld(const DirectX::XMFLOAT3& offset)
{
    m_position.x += offset.x;
    m_position.y += offset.y;
    m_position.z += offset.z;

    SetDirty();
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

void Transform::SetRotation(const XMFLOAT4& quaternion)
{
    m_rotation = quaternion;
    SetDirty();
}

void Transform::SetRotationFromEulerDegrees(const DirectX::XMFLOAT3& euler)
{
    XMStoreFloat4(&m_rotation, XMQuaternionRotationRollPitchYaw(DirectX::XMConvertToRadians(euler.x), DirectX::XMConvertToRadians(euler.y), DirectX::XMConvertToRadians(euler.z)));
    SetDirty();
}

void Transform::SetGlobalRotation(DirectX::XMFLOAT4 quaternion)
{
    XMVECTOR vecQuat = XMLoadFloat4(&quaternion);
    XMVECTOR vecGlobalQuat = XMQuaternionIdentity();

    if (m_parent != nullptr)
    {
        XMVECTOR scale;
        XMVECTOR translation;
        XMMatrixDecompose(&scale, &vecGlobalQuat, &translation, m_parent->GetWorldMatrix());
        vecGlobalQuat = XMQuaternionConjugate(vecGlobalQuat);
    }

    vecQuat = XMQuaternionMultiply(vecGlobalQuat, vecQuat);
    XMStoreFloat4(&quaternion, vecQuat);

    SetRotation(quaternion);

}

void Transform::SetGlobalRotationFromEulerDegrees(const DirectX::XMFLOAT3& euler)
{
    XMVECTOR vecQuat = XMQuaternionRotationRollPitchYaw(DirectX::XMConvertToRadians(euler.x), DirectX::XMConvertToRadians(euler.y), DirectX::XMConvertToRadians(euler.z));

    XMFLOAT4 quat;

    XMStoreFloat4(&quat, vecQuat);

    SetGlobalRotation(quat);
}

void Transform::RotateLocal(const DirectX::XMFLOAT3& rotation)
{
    XMVECTOR curr = XMLoadFloat4(&m_rotation);
    curr = XMQuaternionNormalize(curr);

    XMVECTOR toRotate = XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
    toRotate = XMQuaternionNormalize(toRotate);

    curr = XMQuaternionMultiply(toRotate, curr);

    XMFLOAT4 fRotation;
    XMStoreFloat4(&fRotation, curr);

    SetRotation(fRotation);
}

//TO FIX
void Transform::RotateGlobal(const DirectX::XMFLOAT3& rotation)
{
    XMVECTOR curr = XMLoadFloat4(&m_rotation);
    curr = XMQuaternionNormalize(curr);

    XMVECTOR toRotate = XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
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

void Transform::SetParent(Transform* const& parent, bool preserveTransform)
{
    XMMATRIX matrix = GetWorldMatrix();

    if (m_parent != nullptr)
    {
        size_t removedElements = m_parent->m_children.erase(this);
        assert(removedElements == 1);
    }

    m_parent = parent;

    if (m_parent != nullptr)
    {
        bool success = m_parent->m_children.insert(this).second;
        assert(success);
    }

    if (preserveTransform)
    {
        XMVECTOR vecScale;
        XMVECTOR vecRotation;
        XMVECTOR vecPosition;

        XMFLOAT3 scale;
        XMFLOAT4 rotation;
        XMFLOAT3 position;

        XMMatrixDecompose(&vecScale, &vecRotation, &vecPosition, matrix);

        XMStoreFloat3(&scale, vecScale);
        XMStoreFloat4(&rotation, vecRotation);
        XMStoreFloat3(&position, vecPosition);

        SetGlobalPosition(position);
        SetGlobalScale(scale);
        SetGlobalRotation(rotation);
    }
    else
        SetDirty();
}

void Transform::SetFromMatrix(const XMMATRIX& matrix)
{
    XMVECTOR vecScale;
    XMVECTOR vecRotation;
    XMVECTOR vecPosition;

    XMFLOAT3 scale;
    XMFLOAT4 rotation;
    XMFLOAT3 position;

    XMMatrixDecompose(&vecScale, &vecRotation, &vecPosition, matrix);

    XMStoreFloat3(&scale, vecScale);
    XMStoreFloat4(&rotation, vecRotation);
    XMStoreFloat3(&position, vecPosition);

    SetPosition(position);
    SetScale(scale);
    SetRotation(rotation);
}

Transform* Transform::TryToFindChildWithName(const std::string & name)
{
    for (Transform* const& child : m_children)
    {
        if (child->GetOwner()->Name == name)
            return child;

        Transform* result = child->TryToFindChildWithName(name);

        if (result != nullptr)
            return result;
    }

    return nullptr;
}

void Transform::SetDirty()
{
    m_isDirty = true;

    for (Transform* const& child : m_children)
    {
        child->SetDirty();
    }
}

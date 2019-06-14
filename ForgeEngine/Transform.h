#pragma once
#include <DirectXMath.h>

#include "Component.h"

class Transform : public Component
{
public:
    Transform(Object* owner);
    virtual ~Transform();
    DirectX::XMMATRIX GetWorldMatrix();
    void SetPosition(const DirectX::XMFLOAT3& pos);
    void SetPosition(const float& x, const float& y, const float& z);

    void Translate(const float& x, const float& y, const float& z);
    void TranslateInWorld(const float& x, const float& y, const float& z);

    inline DirectX::XMFLOAT3 GetPosition() const { return m_position; }
    inline DirectX::XMFLOAT4 GetRotation() const { return m_rotation; }
    DirectX::XMFLOAT3 GetRotationAsEuler() const;

    void SetRotation(const DirectX::XMFLOAT4& quaternion);
    void SetRotation(const float& x, const float& y, const float& z);
    inline void SetRotationFromEuler(const float& x, const float& y, const float& z) { SetRotation(DirectX::XMConvertToRadians(x), DirectX::XMConvertToRadians(y), DirectX::XMConvertToRadians(z)); }
    inline void SetRotationFromEuler(const DirectX::XMFLOAT3& euler) { SetRotationFromEuler(euler.x, euler.y, euler.z); }

    void RotateLocal(const float& x, const float& y, const float& z);
    void RotateGlobal(const float& x, const float& y, const float& z);

    void LookAt(const DirectX::XMFLOAT3& target);

private:
    DirectX::XMFLOAT3 m_position;
    DirectX::XMFLOAT4 m_rotation;

    mutable DirectX::XMMATRIX m_worldMatrix;
    mutable bool m_isDirty;
};


#pragma once
#include <DirectXMath.h>

#include "Component.h"

class Transform : public Component
{
public:
    Transform(const DirectX::XMFLOAT3& pos);
    virtual ~Transform();
    DirectX::XMMATRIX GetWorldMatrix();
    void SetPosition(const DirectX::XMFLOAT3& pos);
    void SetPosition(const float& x, const float& y, const float& z);

    inline DirectX::XMFLOAT3 GetPosition() { return m_position; }
    inline DirectX::XMFLOAT4 GetRotation() { return m_rotation; }

    void SetRotation(const DirectX::XMFLOAT4& quaternion);
    void SetRotation(const float& x, const float& y, const float& z);
    inline void SetRotationFromEuler(const float& x, const float& y, const float& z) { SetRotation(DirectX::XMConvertToRadians(x), DirectX::XMConvertToRadians(y), DirectX::XMConvertToRadians(z)); }

    void LookAt(const DirectX::XMFLOAT3& target);

private:
    DirectX::XMFLOAT3 m_position;
    DirectX::XMFLOAT4 m_rotation;

    mutable DirectX::XMMATRIX m_worldMatrix;
    mutable bool m_isDirty;
};


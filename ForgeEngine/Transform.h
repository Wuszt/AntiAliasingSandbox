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


    void SetRotation(const float& x, const float& y, const float& z);
    inline void SetRotationFromEuler(const float& x, const float& y, const float& z) { SetRotation(DirectX::XMConvertToRadians(x), DirectX::XMConvertToRadians(y), DirectX::XMConvertToRadians(z)); }

private:
    DirectX::XMVECTOR m_position;
    DirectX::XMVECTOR m_rotation;

    mutable DirectX::XMMATRIX m_worldMatrix;
    mutable bool m_isDirty;
};


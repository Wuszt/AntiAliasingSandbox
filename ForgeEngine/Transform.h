#pragma once
#include <DirectXMath.h>
#include <unordered_set>

#include "Component.h"

class Transform : public Component
{
public:
    Transform(Object* owner);
    virtual ~Transform();
    DirectX::XMMATRIX GetWorldMatrix();

    void SetScale(const DirectX::XMFLOAT3& scale);
    void SetGlobalScale(DirectX::XMFLOAT3 scale);

    void SetPosition(const DirectX::XMFLOAT3& pos);
    void SetGlobalPosition(DirectX::XMFLOAT3 pos);

    void Translate(const DirectX::XMFLOAT3& offset);
    void TranslateInWorld(const DirectX::XMFLOAT3& offset);

    inline DirectX::XMFLOAT3 GetPosition() const { return m_position; }
    inline DirectX::XMFLOAT4 GetRotation() const { return m_rotation; }
    DirectX::XMFLOAT3 GetRotationAsEuler() const;

    void SetRotation(const DirectX::XMFLOAT4& quaternion);
    void SetRotationFromEulerDegrees(const DirectX::XMFLOAT3& euler);

    void SetGlobalRotation(DirectX::XMFLOAT4 quaternion);
    void SetGlobalRotationFromEulerDegrees(const DirectX::XMFLOAT3& euler);

    void RotateLocal(const DirectX::XMFLOAT3& rotation);
    void RotateGlobal(const DirectX::XMFLOAT3& rotation);

    void LookAt(const DirectX::XMFLOAT3& target);

    void SetParent(Transform* const& parent, bool preserveTransform = false);

private:
    void SetDirty();

    DirectX::XMFLOAT3 m_scale;
    DirectX::XMFLOAT3 m_position;
    DirectX::XMFLOAT4 m_rotation;

    Transform* m_parent;
    std::unordered_set<Transform*> m_children;

    mutable DirectX::XMMATRIX m_worldMatrix;
    mutable bool m_isDirty;
};


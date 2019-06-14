#pragma once
#include <DirectXMath.h>

class Transform;

class Object
{
public:
    Object(DirectX::XMFLOAT3 pos);
    virtual ~Object();

    inline Transform* GetTransform() { return m_transform; }
    virtual void Update() {}

protected:
    Transform* m_transform;
};


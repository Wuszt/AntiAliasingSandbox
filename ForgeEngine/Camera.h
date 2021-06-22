#pragma once
#include <DirectXMath.h>

#include "Object.h"

class Camera : public Object
{
public:
    Camera();
    virtual ~Camera();

    DirectX::XMMATRIX GetViewMatrix();
    inline DirectX::XMMATRIX GetProjectionMatrix() const { return m_projectionMatrixWithOffset; }
    void Initialize(const float& fov, const float& aspectRatio, const float& nearClip, const float& farClip);

    virtual void Update() override;

    void SetOffset(const DirectX::XMFLOAT2& offset);
    DirectX::XMFLOAT2 GetOffset() const { return m_offset; }

    void SetSavedPosition(int i);

private:
    DirectX::XMFLOAT2 m_offset = DirectX::XMFLOAT2();
    DirectX::XMMATRIX m_projectionMatrix;
    DirectX::XMMATRIX m_projectionMatrixWithOffset;
};


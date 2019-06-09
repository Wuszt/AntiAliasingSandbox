#pragma once
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

class Camera
{
public:
    Camera(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& lookAt, const float& fov, const float& aspectRatio, const float& nearClip, const float& farClip);
    virtual ~Camera();

    void SetCamPos(float x, float y, float z);
    void SetLookAt(float x, float y, float z);

private:
    DirectX::XMMATRIX m_viewMatrix;
    bool m_isViewDirty = true;

    DirectX::XMMATRIX m_projectionMatrix;

    DirectX::XMVECTOR m_camPos;
    DirectX::XMVECTOR m_camLookAt;
    DirectX::XMVECTOR m_camUp;

public:
    DirectX::XMMATRIX GetViewMatrix();
    inline DirectX::XMMATRIX GetProjectionMatrix() { return m_projectionMatrix; }
};


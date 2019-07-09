#include "DirectionalLight.h"

using namespace DirectX;

DirectionalLight::DirectionalLight(Object* const& owner, const XMFLOAT3& dir, const XMFLOAT3& color, const float& intensity) : Light(owner)
{
    XMVECTOR tmp = XMLoadFloat3(&dir);
    XMVector3Normalize(tmp);
    XMStoreFloat3(&m_currentData.Direction, tmp);

    m_currentData.Color.x = color.x * intensity;
    m_currentData.Color.y = color.y * intensity;
    m_currentData.Color.z = color.z * intensity;
}

cbDirectionalLight& DirectionalLight::GetData()
{
    return m_currentData;
}

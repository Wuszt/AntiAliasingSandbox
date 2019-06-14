#include "ControllableCamera.h"
#include "InputClass.h"
#include "Time.h"
#include "Transform.h"

ControllableCamera::~ControllableCamera()
{
}

void ControllableCamera::Update()
{
    float horizontal = ((InputClass::GetKey(DIK_D) ? 1 : 0) - (InputClass::GetKey(DIK_A) ? 1 : 0)) * Time::GetDeltaTime() * 5.0f;
    float vertical = ((InputClass::GetKey(DIK_W) ? 1 : 0) - (InputClass::GetKey(DIK_S) ? 1 : 0)) * Time::GetDeltaTime() * 5.0f;

    if (InputClass::IsMouseButtonDown(1))
    {
        XMFLOAT3 delta = InputClass::GetMouseDeltaPosition();
        float sensitivity = 0.001f;
        m_transform->RotateLocal({ delta.y * sensitivity, 0.0f, 0.0f });
        m_transform->RotateGlobal({ 0.0f, delta.x * sensitivity, 0.0f });
        vertical += delta.z * 0.025f;
    }

    m_transform->Translate({ horizontal, 0.0f, vertical });
}

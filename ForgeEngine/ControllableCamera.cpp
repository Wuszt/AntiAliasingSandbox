#include "ControllableCamera.h"
#include "InputClass.h"
#include "Time.h"
#include "Transform.h"

ControllableCamera::~ControllableCamera()
{
}

void ControllableCamera::Update()
{
    Camera::Update();

    float horizontal = ((InputClass::GetKey(DIK_D) ? 1 : 0) - (InputClass::GetKey(DIK_A) ? 1 : 0)) * Time::GetDeltaTime() * 5.0f;
    float vertical = ((InputClass::GetKey(DIK_W) ? 1 : 0) - (InputClass::GetKey(DIK_S) ? 1 : 0)) * Time::GetDeltaTime() * 5.0f;

    float rotationHorizontal = ((InputClass::GetKey(DIK_RIGHT) ? 1 : 0) - (InputClass::GetKey(DIK_LEFT) ? 1 : 0)) * Time::GetDeltaTime() * 2.0f;
    float rotationVertical = ((InputClass::GetKey(DIK_DOWN) ? 1 : 0) - (InputClass::GetKey(DIK_UP) ? 1 : 0)) * Time::GetDeltaTime() * 2.0f;

    if (InputClass::IsMouseButtonDown(1))
    {
        XMFLOAT3 delta = InputClass::GetMouseDeltaPosition();
        float sensitivity = 0.001f;
        rotationHorizontal += delta.x * sensitivity;
        rotationVertical += delta.y * sensitivity;
        vertical += delta.z * 0.025f;
    }

    m_transform->RotateLocal({ rotationVertical, 0.0f, 0.0f });
    m_transform->RotateGlobal({ 0.0f, rotationHorizontal, 0.0f });

    m_transform->Translate({ horizontal, 0.0f, vertical });
}

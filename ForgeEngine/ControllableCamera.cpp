#include "ControllableCamera.h"
#include "InputClass.h"
#include "Time.h"
#include "Transform.h"
#include <sstream>

ControllableCamera::~ControllableCamera()
{
}

void ControllableCamera::Update()
{
    float horizontal = ((InputClass::GetKey(DIK_D) ? 1 : 0) - (InputClass::GetKey(DIK_A) ? 1 : 0)) * Time::GetDeltaTime() * 5.0f;
    float vertical = ((InputClass::GetKey(DIK_W) ? 1 : 0) - (InputClass::GetKey(DIK_S) ? 1 : 0)) * Time::GetDeltaTime() * 5.0f;

    m_transform->Translate(horizontal, 0.0f, vertical);
}

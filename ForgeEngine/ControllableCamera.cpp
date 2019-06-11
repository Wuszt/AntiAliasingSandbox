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


    float horizontalR = ((InputClass::GetKey(DIK_RIGHTARROW) ? 1 : 0) - (InputClass::GetKey(DIK_LEFTARROW) ? 1 : 0)) * 0.001f;
    float verticalR = ((InputClass::GetKey(DIK_UPARROW) ? 1 : 0) - (InputClass::GetKey(DIK_DOWNARROW) ? 1 : 0)) * 0.001f;

    XMFLOAT3 rot = GetTransform()->GetRotationAsEuler();

    std::ostringstream ss;
    ss << "(" << XMConvertToDegrees(rot.x) << "," << XMConvertToDegrees(rot.y) << "," << XMConvertToDegrees(rot.z) << ")";
    rot.y += horizontalR;

    ss << " | ";
    ss << "(" << XMConvertToDegrees(rot.x) << "," << XMConvertToDegrees(rot.y) << "," << XMConvertToDegrees(rot.z) << ")";
    ss << "\n";
    std::string s(ss.str());
    OutputDebugString(s.c_str());


    GetTransform()->SetRotationFromEuler(rot);
    float tmp = GetTransform()->GetRotationAsEuler().y - rot.y;
    int i = 5;

    //if (InputClass::IsMouseButtonDown(1))
    //{
    //    float speed = 0.001f;
    //    XMFLOAT3 delta = InputClass::GetMouseDeltaPosition();
    //    XMFLOAT3 rot = m_transform->GetRotationAsEuler();

    //    std::ostringstream ss;
    //    ss << XMConvertToDegrees(rot.x);
    //    ss << "\n";
    //    std::string s(ss.str());

    //    OutputDebugString(s.c_str());

    //    rot.x += delta.y * 0.001f;
    //    rot.y += delta.x * 0.001f;

    //    m_transform->SetRotation(rot.x, rot.y, 0.0f);
    //    // m_transform->Rotate(0.0f, delta.x * 0.001f, 0.0f);
    //}

    //m_transform->Translate(horizontal, 0.0f, vertical);
}

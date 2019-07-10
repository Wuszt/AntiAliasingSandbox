#include "MyApp.h"
#include "MeshRenderer.h"
#include "Object.h"
#include "Transform.h"
#include "DirectionalLight.h"
#include <DirectXCommonClasses/Time.h>

using namespace DirectX;

void MyApp::InitScene()
{
    Core::InitScene();

    m_car = InstantiateObject<Object>();
    m_car->GetTransform()->SetPosition({ 0.0f, 0.0f, 0.0f });
    m_car->GetTransform()->SetScale({ 0.01f, 0.01f, 0.01f });
    m_car->AddComponent<MeshRenderer>("model.fbx", "Base.fx");

    Object* light0 = InstantiateObject<Object>();
    light0->AddComponent<DirectionalLight>(XMFLOAT3(0.5f, -1.0f, -0.5f), XMFLOAT3(1.0f, 1.0f, 1.0f) , 0.75f);

    //Object* light1 = InstantiateObject<Object>();
    //light1->AddComponent<DirectionalLight>(XMFLOAT3(1.0f, -0.2f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 0.25f);

    //Object* light2 = InstantiateObject<Object>();
    //light2->AddComponent<DirectionalLight>(XMFLOAT3(0.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 0.5f);
}

void MyApp::UpdateScene()
{
    Core::UpdateScene();
}

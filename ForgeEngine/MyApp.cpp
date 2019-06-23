#include "MyApp.h"
#include "MeshRenderer.h"
#include "Object.h"
#include "Transform.h"

void MyApp::InitScene()
{
    Core::InitScene();

    Object* obj = InstantiateObject<Object>();
    obj->GetTransform()->SetPosition({ 0.0f, 0.0f, 0.0f });
    obj->GetTransform()->SetScale({ 0.01f, 0.01f, 0.01f });
    obj->AddComponent<MeshRenderer>("model.fbx");
}

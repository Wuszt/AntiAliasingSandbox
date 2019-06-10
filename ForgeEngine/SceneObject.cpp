#include "SceneObject.h"
#include "Transform.h"

using namespace DirectX;


SceneObject::SceneObject(XMFLOAT3 pos)
{
    m_transform = new Transform(pos);
}


SceneObject::~SceneObject()
{
    delete m_transform;
}

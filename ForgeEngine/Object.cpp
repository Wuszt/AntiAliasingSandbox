#include "Object.h"
#include "Transform.h"

using namespace DirectX;


Object::Object()
{
    m_transform = AddComponent<Transform>();
}


Object::~Object()
{
    delete m_transform;
}

#include "Object.h"
#include "Transform.h"

using namespace DirectX;


Object::Object()
{
    m_transform = AddComponent<Transform>();
    Name = "Object";
}


Object::~Object()
{
    delete m_transform;
}

void Object::Update()
{
    if (!m_started)
    {
        Start();
        m_started = true;
    }
}

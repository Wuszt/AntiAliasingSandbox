#include "Object.h"
#include "Transform.h"
#include "Component.h"

using namespace DirectX;


Object::Object()
{
    m_transform = AddComponent<Transform>();
    Name = "Object";
}


Object::~Object()
{
    for (Component* const& comp : m_components)
        delete comp;
}

void Object::Update()
{
    if (!m_started)
    {
        Start();
        m_started = true;
    }
}

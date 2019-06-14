#include "Component.h"

Component::Component(Object* owner)
{
    m_owner = owner;
}

Component::~Component()
{
}

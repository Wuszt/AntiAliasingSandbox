#include "Object.h"
#include "Transform.h"

using namespace DirectX;


Object::Object(XMFLOAT3 pos)
{
    m_transform = new Transform(pos);
}


Object::~Object()
{
    delete m_transform;
}

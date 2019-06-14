#pragma once
#include "Component.h"

class Object;

class MeshRenderer : public Component
{
public:
    MeshRenderer(Object* const& owner) : Component(owner) {}
    ~MeshRenderer();

    virtual void Render() {}
};


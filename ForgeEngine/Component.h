#pragma once

class Object;

class Component
{
public:
    Component(Object* owner);
    virtual ~Component();

private:
    Object* m_owner;
};


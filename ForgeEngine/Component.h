#pragma once

class Object;

class Component
{
public:
    Component(Object* owner);
    virtual ~Component();

    inline Object* GetOwner() { return m_owner; }

private:
    Object* m_owner;
};


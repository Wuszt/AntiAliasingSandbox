#pragma once

class Object;

class Component
{
public:
    Component(Object* owner);
    virtual ~Component();

    inline Object* GetOwner() const { return m_owner; }

private:
    Object* m_owner;
};


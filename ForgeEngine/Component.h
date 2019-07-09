#pragma once

class Object;

class Component
{
public:
    Component(Object* owner);
    virtual ~Component();

    inline Object* GetOwner() const { return m_owner; }

    virtual void OnInitialized() {}

private:
    Object* m_owner;
};


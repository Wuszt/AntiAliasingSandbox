#pragma once
#include "Object.h"
class Car : public Object
{
public:
    Car();
    ~Car();

    virtual void Start() override;
    virtual void Update() override;

private:
    Transform* m_wheelRR;
    Transform* m_wheelRL;
    Transform* m_wheelFR;
    Transform* m_wheelFL;
};


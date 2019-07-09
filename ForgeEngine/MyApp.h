#pragma once
#include "Core.h"
class MyApp : public Core
{
public:
    using Core::Core;

protected:
    virtual void InitScene() override;
    virtual void UpdateScene() override;

    Object* m_car;
};


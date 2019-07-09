#pragma once
#include "Component.h"
class Light : public Component
{
public:
    using Component::Component;
    virtual void OnInitialized() override;
};


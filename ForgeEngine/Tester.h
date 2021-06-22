#pragma once
#include "MyApp.h"
class Tester : public MyApp
{
public:
    using MyApp::MyApp;

protected:
    virtual void UpdateScene() override;
private:
    AAMode m_currentAAMode;
    int m_framesCounter = -1000;
    int m_currentCameraPos = 1;
    int m_currentVariant = 0;
    float m_time = 0.0f;
};


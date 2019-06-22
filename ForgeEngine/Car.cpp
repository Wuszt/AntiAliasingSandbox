#include "Car.h"
#include "Transform.h"
#include "Time.h"

Car::Car()
{
    Name = "Car";
}


Car::~Car()
{
}

void Car::Start()
{
    m_wheelRR = GetTransform()->TryToFindChildWithName("Wheel_RR");
    m_wheelFR = GetTransform()->TryToFindChildWithName("Wheel_FR");
    m_wheelRL = GetTransform()->TryToFindChildWithName("Wheel_RL");
    m_wheelFL = GetTransform()->TryToFindChildWithName("Wheel_FL");
}

void Car::Update()
{
    Object::Update();

    m_wheelFR->SetGlobalRotationFromEulerDegrees({ Time::GetTime() * 1000.0f, 0.0f, 0.0f });
}

#include "Light.h"
#include "Core.h"
#include "LightsManager.h"

void Light::OnInitialized()
{
    Core::GetLightsManager()->AddLight(this);
}

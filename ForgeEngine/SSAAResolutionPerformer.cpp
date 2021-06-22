#include "SSAAResolutionPerformer.h"
#include <DirectXCommonClasses/InputClass.h>
#include <string>
#include <functional>
#include "Core.h"
#include "Window.h"


SSAAResolutionPerformer::SSAAResolutionPerformer(std::function<void(RTV*)> func) : IAAPerformer(func)
{

}

SSAAResolutionPerformer::~SSAAResolutionPerformer()
{
}

void SSAAResolutionPerformer::OnEnable()
{
    IAAPerformer::OnEnable();
    m_resolutionMultiplier = 2;

    m_baseResolution.x = (float)Core::GetWindow()->GetResolutionWidth();
    m_baseResolution.y = (float)Core::GetWindow()->GetResolutionHeight();

    Core::GetWindow()->SetResolution((int)m_baseResolution.x * m_resolutionMultiplier, (int)m_baseResolution.y * m_resolutionMultiplier);
}

void SSAAResolutionPerformer::OnDisable()
{
    Core::GetWindow()->SetResolution((int)m_baseResolution.x, (int)m_baseResolution.y);
}

void SSAAResolutionPerformer::Update()
{
    IAAPerformer::Update();

    if (InputClass::GetKeyDown(DIK_Q))
    {
        m_resolutionMultiplier *= 2;

        if (m_resolutionMultiplier > 8)
            m_resolutionMultiplier = 2;

        Core::GetWindow()->SetResolution((int)m_baseResolution.x * m_resolutionMultiplier, (int)m_baseResolution.y * m_resolutionMultiplier);
    }
}

std::string SSAAResolutionPerformer::GetName()
{
    return "SSAA Resolution x" + std::to_string(m_resolutionMultiplier);
}

void SSAAResolutionPerformer::DrawScene()
{
    m_drawFunc(m_output);
}

void SSAAResolutionPerformer::PostProcessing()
{
}

void SSAAResolutionPerformer::SetVariant(int variantIndex)
{
    m_resolutionMultiplier = (int)pow(2, variantIndex + 2);

    Core::GetWindow()->SetResolution((int)m_baseResolution.x * m_resolutionMultiplier, (int)m_baseResolution.y * m_resolutionMultiplier);
}

#include "FXAAPerformer.h"
#include "Core.h"
#include "RenderTargetViewsManager.h"
#include "PostProcessor.h"
#include "ShadersManager.h"

FXAAPerformer::FXAAPerformer(std::function<void(RTV*)> func) : IAAPerformer(func)
{
    ShadersManager::GetShadersManager()->GetShaders("FXAA.fx");
}

void FXAAPerformer::DrawScene()
{
    m_drawFunc(m_output);
}

void FXAAPerformer::PostProcessing()
{
    RTV* desaturated = Core::GetRTVsManager()->AcquireRTV(SizeType::Resolution);
    RTV* tmp = Core::GetRTVsManager()->AcquireRTV(SizeType::Resolution);

    PostProcessor::DrawPass("DesaturationPP.fx", { m_output }, desaturated);
    PostProcessor::DrawPass("FXAA.fx", { m_output, desaturated }, tmp);

    Core::GetRTVsManager()->ReleaseRTV(desaturated);
    Core::GetRTVsManager()->ReleaseRTV(m_output);
    m_output = tmp;
}

std::string FXAAPerformer::GetName()
{
    return "FXAA";
}
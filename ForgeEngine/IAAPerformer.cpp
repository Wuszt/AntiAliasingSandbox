#include "IAAPerformer.h"
#include "DebugLog.h"
#include "Core.h"
#include "RenderTargetViewsManager.h"

IAAPerformer::IAAPerformer(std::function<void(RTV*)> func)
{
    m_drawFunc = func;
}

IAAPerformer::~IAAPerformer() = default;

void IAAPerformer::OnEnable()
{
    m_output = Core::GetRTVsManager()->AcquireRTV(SizeType::Resolution);
}

void IAAPerformer::OnDisable()
{
    Core::GetRTVsManager()->ReleaseRTV(m_output);
}

void IAAPerformer::Update()
{
    DebugLog::Log(GetName());
}

RTV* IAAPerformer::GetOutputRTV()
{
    return m_output;
}

#include "DummyAAPerformer.h"

DummyAAPerformer::DummyAAPerformer(std::function<void(RTV*)> func) : IAAPerformer(func)
{

}

void DummyAAPerformer::DrawScene()
{
    m_drawFunc(m_output);
}

void DummyAAPerformer::PostProcessing()
{
}

std::string DummyAAPerformer::GetName()
{
    return "NO AA";
}

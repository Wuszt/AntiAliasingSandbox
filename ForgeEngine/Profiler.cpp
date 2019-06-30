#include "Profiler.h"
#include "DebugLog.h"
#include "Window.h"
#include <DirectXMath.h>
#include "RenderingSystem.h"
#include <string>
#include "ProfilingSession.h"

using namespace DirectX;
using namespace std;

Profiler::Profiler(const RenderingSystem* const& renderingSystem, const Window* const& window)
{
    m_renderingSystem = renderingSystem;
    m_window = window;
    QueryPerformanceFrequency(&m_CPUfrequency);
    m_CPUfrequency.QuadPart /= 1000;
}


Profiler::~Profiler()
{
}

void Profiler::Initialize(const RenderingSystem* const& renderingSystem, const Window* const& window)
{
    s_instance = new Profiler(renderingSystem, window);
}

void Profiler::Release()
{
    delete s_instance;
}

void Profiler::Draw()
{
    s_instance->OnDraw();
}

void Profiler::StartCPUProfiling(const std::string& name)
{
    s_instance->OnStartCPUProfiling(name);
}

void Profiler::EndCPUProfiling(const std::string& name)
{
    s_instance->OnEndCPUProfiling(name);
}

void Profiler::StartFrame()
{
    s_instance->OnStartFrame();
}

void Profiler::EndFrame()
{
    s_instance->OnEndFrame();
}

Profiler* Profiler::s_instance;

void Profiler::OnStartCPUProfiling(const std::string& name)
{
    auto found = m_cpuProfilers.find(name);

    CPUProfilingSession* session;

    if (found == m_cpuProfilers.end())
    {
        session = m_cpuProfilers.emplace(name, new CPUProfilingSession(SAMPLES_AMOUNT)).first->second;
    }
    else
        session = found->second;

    session->OnStartProfiling(m_currentCPUSession, m_orderCounter++);

    m_currentCPUSession = session;
}

void Profiler::OnEndCPUProfiling(const std::string& name)
{
    CPUProfilingSession* session = m_cpuProfilers[name];
    m_currentCPUSession = session->GetParent();
    session->OnEndProfiling();
}

void Profiler::OnDraw()
{
    int i = 0;
    for (const auto& mess : m_cachedMessages)
    {
        ++i;
        m_renderingSystem->DrawText(mess, PA_TEXT_SIZE, s_instance->m_window->GetWidth() * 0.5f, i * PA_TEXT_SIZE, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), TextAnchor::Top | TextAnchor::Left);
    }

    m_cachedMessages.clear();
}

void Profiler::OnStartFrame()
{
    m_currentCPUSession = nullptr;
    m_orderCounter = 0;
}

void Profiler::OnEndFrame()
{
    m_cachedMessages.resize(m_cpuProfilers.size());
    for (const auto& profiler : m_cpuProfilers)
    {
        static double result;
        result = profiler.second->GetAverageResult() / m_CPUfrequency.QuadPart;

        string spaces = "";
        const ProfilingSession* prof = profiler.second;
        while ((prof = prof->GetParent()) && prof->GetParent())
        {
            spaces += "|        ";
        }

        if (profiler.second->GetParent())
            spaces += "|-----";

        m_cachedMessages[profiler.second->GetOrder()] = spaces + profiler.first + ": " + to_string(result) + "ms";
    }
}

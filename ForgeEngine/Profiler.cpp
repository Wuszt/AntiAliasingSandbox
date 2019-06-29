#include "Profiler.h"
#include "DebugLog.h"
#include "Window.h"
#include <DirectXMath.h>
#include "RenderingSystem.h"
#include <string>

using namespace DirectX;
using namespace std;

Profiler::Profiler(const RenderingSystem* const& renderingSystem, const Window* const& window)
{
    m_renderingSystem = renderingSystem;
    m_window = window;
    QueryPerformanceFrequency(&m_frequency);
    m_frequency.QuadPart /= 1000;
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
    const auto& frame = s_instance->m_results.find(FRAME_ANALYZE_NAME);

    if (frame != s_instance->m_results.end())
        s_instance->m_renderingSystem->DrawText("[CPU] Frame took: " + to_string(frame->second) + "ms", PA_TEXT_SIZE, s_instance->m_window->GetWidth() * 0.5f, 0.0f, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), TextAnchor::Top | TextAnchor::Center);

    int i = 1;
    for (const auto& pr : s_instance->m_results)
    {
        if(pr.first == FRAME_ANALYZE_NAME)
            continue;

        s_instance->m_renderingSystem->DrawText("[CPU] " + pr.first + " took: " + to_string(pr.second) + "ms", PA_TEXT_SIZE, s_instance->m_window->GetWidth() * 0.5f, i * PA_TEXT_SIZE, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), TextAnchor::Top | TextAnchor::Center);
        ++i;
    }

    s_instance->m_results.clear();
}

void Profiler::StartProfiling(const std::string& name)
{
    auto found = s_instance->m_profilers.find(name);

    if (found != s_instance->m_profilers.end())
    {
        DebugLog::LogError(name + " analyzer already started");
        return;
    }

    auto it = s_instance->m_profilers.emplace(name, LARGE_INTEGER());
    QueryPerformanceCounter(&it.first->second);
}

double Profiler::EndProfiling(const std::string& name)
{
    static LARGE_INTEGER current;

    auto found = s_instance->m_profilers.find(name);

    if (found == s_instance->m_profilers.end())
    {
        DebugLog::LogError(name + " analyzer haven't been started");
    }

    QueryPerformanceCounter(&current);

    double duration = static_cast<double>(current.QuadPart - found->second.QuadPart) / s_instance->m_frequency.QuadPart;

    s_instance->m_profilers.erase(name);

    auto foundResult = s_instance->m_results.find(name);

    if (foundResult != s_instance->m_results.end())
        foundResult->second += duration;
    else
        s_instance->m_results.emplace(name, duration);

    return duration;
}

Profiler* Profiler::s_instance;

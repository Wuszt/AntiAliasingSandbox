#include "PerformanceAnalyzer.h"
#include "DebugLog.h"
#include "Window.h"
#include <DirectXMath.h>
#include "RenderingSystem.h"
#include <string>

using namespace DirectX;
using namespace std;

PerformanceAnalyzer::PerformanceAnalyzer(const RenderingSystem* const& renderingSystem, const Window* const& window)
{
    m_renderingSystem = renderingSystem;
    m_window = window;
    QueryPerformanceFrequency(&m_frequency);
    m_frequency.QuadPart /= 1000;
}


PerformanceAnalyzer::~PerformanceAnalyzer()
{
}

void PerformanceAnalyzer::Initialize(const RenderingSystem* const& renderingSystem, const Window* const& window)
{
    s_instance = new PerformanceAnalyzer(renderingSystem, window);
}

void PerformanceAnalyzer::Release()
{
    delete s_instance;
}

void PerformanceAnalyzer::Draw()
{
    const auto& frame = s_instance->m_results.find(FRAME_ANALYZE_NAME);

    if (frame != s_instance->m_results.end())
        s_instance->m_renderingSystem->DrawText("Frame took: " + to_string(frame->second) + "ms", PA_TEXT_SIZE, s_instance->m_window->GetWidth() * 0.5f, 0.0f, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), TextAnchor::Top | TextAnchor::Center);

    int i = 1;
    for (const auto& pr : s_instance->m_results)
    {
        if(pr.first == FRAME_ANALYZE_NAME)
            continue;

        s_instance->m_renderingSystem->DrawText(pr.first + " took: " + to_string(pr.second) + "ms", PA_TEXT_SIZE, s_instance->m_window->GetWidth() * 0.5f, i * PA_TEXT_SIZE, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), TextAnchor::Top | TextAnchor::Center);
        ++i;
    }

    s_instance->m_results.clear();
}

void PerformanceAnalyzer::StartAnalyzing(const std::string& name)
{
    auto found = s_instance->m_analyzers.find(name);

    if (found != s_instance->m_analyzers.end())
    {
        DebugLog::LogError(name + " analyzer already started");
        return;
    }

    auto it = s_instance->m_analyzers.emplace(name, LARGE_INTEGER());
    QueryPerformanceCounter(&it.first->second);
}

double PerformanceAnalyzer::FinishAnalyzing(const std::string& name)
{
    static LARGE_INTEGER current;

    auto found = s_instance->m_analyzers.find(name);

    if (found == s_instance->m_analyzers.end())
    {
        DebugLog::LogError(name + " analyzer haven't been started");
    }

    QueryPerformanceCounter(&current);

    double duration = static_cast<double>(current.QuadPart - found->second.QuadPart) / s_instance->m_frequency.QuadPart;

    s_instance->m_analyzers.erase(name);

    auto foundResult = s_instance->m_results.find(name);

    if (foundResult != s_instance->m_results.end())
        foundResult->second += duration;
    else
        s_instance->m_results.emplace(name, duration);

    return duration;
}

PerformanceAnalyzer* PerformanceAnalyzer::s_instance;

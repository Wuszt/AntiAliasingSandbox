#pragma once
#include <string>
#include <unordered_map>
#include <Windows.h>

#define PA_TEXT_SIZE 20.0f
#define FRAME_ANALYZE_NAME "Frame"

class RenderingSystem;
class Window;

class PerformanceAnalyzer
{
public:
    PerformanceAnalyzer(const RenderingSystem* const& renderingSystem, const Window* const& window);
    ~PerformanceAnalyzer();

    static void Initialize(const RenderingSystem* const& renderingSystem, const Window* const& window);
    static void Release();

    static void Draw();

    static void StartAnalyzing(const std::string& name);
    static double FinishAnalyzing(const std::string& name);

private:
    static PerformanceAnalyzer* s_instance;

    const RenderingSystem* m_renderingSystem;
    const Window* m_window;

    std::unordered_map<std::string, LARGE_INTEGER> m_analyzers;
    std::unordered_map<std::string, double> m_results;
    LARGE_INTEGER m_frequency;
};


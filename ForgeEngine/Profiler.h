#pragma once
#include <string>
#include <unordered_map>
#include <Windows.h>

#define PA_TEXT_SIZE 20.0f
#define FRAME_ANALYZE_NAME "Frame"

class RenderingSystem;
class Window;

class Profiler
{
public:
    Profiler(const RenderingSystem* const& renderingSystem, const Window* const& window);
    ~Profiler();

    static void Initialize(const RenderingSystem* const& renderingSystem, const Window* const& window);
    static void Release();

    static void Draw();

    static void StartProfiling(const std::string& name);
    static double EndProfiling(const std::string& name);

private:
    static Profiler* s_instance;

    const RenderingSystem* m_renderingSystem;
    const Window* m_window;

    std::unordered_map<std::string, LARGE_INTEGER> m_profilers;
    std::unordered_map<std::string, double> m_results;
    LARGE_INTEGER m_frequency;
};


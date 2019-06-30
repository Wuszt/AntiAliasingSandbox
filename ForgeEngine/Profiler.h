#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <Windows.h>

#define SAMPLES_AMOUNT 100000

#define PA_TEXT_SIZE 20.0f
#define FRAME_ANALYZE_NAME "Frame"

class RenderingSystem;
class Window;
class ProfilingSession;
class CPUProfilingSession;

class Profiler
{
public:
    Profiler(const RenderingSystem* const& renderingSystem, const Window* const& window);
    ~Profiler();

    static void Initialize(const RenderingSystem* const& renderingSystem, const Window* const& window);
    static void Release();

    static void Draw();

    static void StartCPUProfiling(const std::string& name);
    static void EndCPUProfiling(const std::string& name);

    static void StartFrame();
    static void EndFrame();

private:
    static Profiler* s_instance;

    const RenderingSystem* m_renderingSystem;
    const Window* m_window;

    void OnStartCPUProfiling(const std::string& name);
    void OnEndCPUProfiling(const std::string& name);
    void OnDraw();
    void OnStartFrame();
    void OnEndFrame();

    std::unordered_map<std::string, CPUProfilingSession*> m_cpuProfilers;
    LARGE_INTEGER m_CPUfrequency;

    std::vector<std::string> m_cachedMessages;
    int m_orderCounter;

    const ProfilingSession* m_currentCPUSession;
};


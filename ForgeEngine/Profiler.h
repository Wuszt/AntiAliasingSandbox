#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <Windows.h>

#define SAMPLES_AMOUNT 500

#define QUERY_LATENCY 5

#define PA_TEXT_SIZE 15.0f
#define FRAME_ANALYZE_NAME "Frame"

class RenderingSystem;
class Window;
class ProfilingSession;
class CPUProfilingSession;
class GPUProfilingSession;

struct ID3D11Device;
struct ID3D11DeviceContext;

struct ID3D11Query;

class Profiler
{
public:
    Profiler(ID3D11Device* const& d3Device, ID3D11DeviceContext* const& d3Context, const RenderingSystem* const& renderingSystem, const Window* const& window);
    ~Profiler();

    static void Initialize(ID3D11Device* const& d3Device, ID3D11DeviceContext* const& d3Context, const RenderingSystem* const& renderingSystem, const Window* const& window);
    static void Release();

    static void Draw();

    static void StartCPUProfiling(const std::string& name);
    static void EndCPUProfiling(const std::string& name);

    static void StartGPUProfiling(const std::string& name);
    static void EndGPUProfiling(const std::string& name);

    static void StartProfiling(const std::string& name);
    static void EndProfiling(const std::string& name);

    static void StartFrame();
    static void EndFrame();

private:
    static Profiler* s_instance;

    const RenderingSystem* m_renderingSystem;
    const Window* m_window;

    void OnStartCPUProfiling(const std::string& name);
    void OnEndCPUProfiling(const std::string& name);

    void OnStartGPUProfiling(const std::string& name);
    void OnEndGPUProfiling(const std::string& name);

    void OnDraw();
    void OnStartFrame();
    void OnEndFrame();

    void PrepareLogsHierarchy(std::unordered_map<std::string, ProfilingSession*>::iterator begin, std::unordered_map<std::string, ProfilingSession*>::iterator end, const int& length, const UINT64& freq);

    std::unordered_map<std::string, ProfilingSession*> m_cpuProfilers;
    LARGE_INTEGER m_CPUfrequency;

    std::unordered_map<std::string, ProfilingSession*> m_gpuProfilers[QUERY_LATENCY];

    std::string m_cachedMessages;
    int m_cpuOrderCounter;
    int m_gpuOrderCounter;

    const ProfilingSession* m_currentCPUSession;
    const ProfilingSession* m_currentGPUSession;

    ID3D11Device* m_d3Device;
    ID3D11DeviceContext* m_d3Context;
    ID3D11Query* m_queryJoints[QUERY_LATENCY];

    int m_framesCounter = 0;
    float m_tmpTime = 0.0f;
    float m_tmpFramesCounter = 0;
    double m_profilingTime = 0;

    int m_currentFPS;
    float m_currentFrameDuration;
};


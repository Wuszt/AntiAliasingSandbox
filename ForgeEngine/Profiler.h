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

    void PrepareLogsHierarchy(std::unordered_map<std::string, ProfilingSession*>::iterator begin, std::unordered_map<std::string, ProfilingSession*>::iterator end, const double& freq, const int& offset);

    std::unordered_map<std::string, ProfilingSession*> m_cpuProfilers;
    LARGE_INTEGER m_CPUfrequency;

    std::unordered_map<std::string, ProfilingSession*> m_gpuProfilers[2];

    std::vector<std::string> m_cachedMessages;
    int m_cpuOrderCounter;
    int m_gpuOrderCounter;

    const ProfilingSession* m_currentCPUSession;
    const ProfilingSession* m_currentGPUSession;

    ID3D11Device* m_d3Device;
    ID3D11DeviceContext* m_d3Context;
    ID3D11Query* m_queryJoints[2];

    int m_frameCounter = 0;
};


#include "Profiler.h"
#include "DebugLog.h"
#include "Window.h"
#include <DirectXMath.h>
#include "RenderingSystem.h"
#include <string>
#include "ProfilingSession.h"
#include <d3d11.h>
#include <DirectXCommonClasses/Time.h>
#include <sstream>

using namespace DirectX;
using namespace std;

Profiler::Profiler(ID3D11Device* const& d3Device, ID3D11DeviceContext* const& d3Context, const RenderingSystem* const& renderingSystem, const Window* const& window)
{
    m_renderingSystem = renderingSystem;
    m_window = window;
    QueryPerformanceFrequency(&m_CPUfrequency);

    m_d3Context = d3Context;
    m_d3Device = d3Device;

    D3D11_QUERY_DESC desc;
    desc.MiscFlags = 0;
    desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;

    for (int i = 0; i < QUERY_LATENCY; ++i)
        d3Device->CreateQuery(&desc, &m_queryJoints[i]);
}

Profiler::~Profiler()
{
}

void Profiler::Initialize(ID3D11Device* const& d3Device, ID3D11DeviceContext* const& d3Context, const RenderingSystem* const& renderingSystem, const Window* const& window)
{
    s_instance = new Profiler(d3Device, d3Context, renderingSystem, window);
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
    static LARGE_INTEGER start, end;
    QueryPerformanceCounter(&start);

    s_instance->OnStartCPUProfiling(name);

    QueryPerformanceCounter(&end);
    s_instance->m_profilingTime += (double)(end.QuadPart - start.QuadPart);
}

void Profiler::EndCPUProfiling(const std::string& name)
{
    static LARGE_INTEGER start, end;
    QueryPerformanceCounter(&start);

    s_instance->OnEndCPUProfiling(name);

    QueryPerformanceCounter(&end);
    s_instance->m_profilingTime += (double)(end.QuadPart - start.QuadPart);
}

void Profiler::StartGPUProfiling(const std::string& name)
{
    static LARGE_INTEGER start, end;
    QueryPerformanceCounter(&start);

    s_instance->OnStartGPUProfiling(name);

    QueryPerformanceCounter(&end);
    s_instance->m_profilingTime += (double)(end.QuadPart - start.QuadPart);
}

void Profiler::EndGPUProfiling(const std::string& name)
{
    static LARGE_INTEGER start, end;
    QueryPerformanceCounter(&start);

    s_instance->OnEndGPUProfiling(name);

    QueryPerformanceCounter(&end);
    s_instance->m_profilingTime += (double)(end.QuadPart - start.QuadPart);
}

void Profiler::StartProfiling(const std::string& name)
{
    StartGPUProfiling(name);
    StartCPUProfiling(name);
}

void Profiler::EndProfiling(const std::string& name)
{
    EndCPUProfiling(name);
    EndGPUProfiling(name);
}

void Profiler::StartFrame()
{
    static LARGE_INTEGER start, end;
    QueryPerformanceCounter(&start);

    s_instance->OnStartFrame();

    QueryPerformanceCounter(&end);
    s_instance->m_profilingTime += (double)(end.QuadPart - start.QuadPart);
}

void Profiler::EndFrame()
{
    static LARGE_INTEGER start, end;
    QueryPerformanceCounter(&start);

    s_instance->OnEndFrame();

    QueryPerformanceCounter(&end);
    s_instance->m_profilingTime += (double)(end.QuadPart - start.QuadPart);
}

Profiler* Profiler::s_instance;

void Profiler::OnStartCPUProfiling(const std::string& name)
{
    auto found = m_cpuProfilers.find(name);

    ProfilingSession* session;

    if (found == m_cpuProfilers.end())
    {
        session = m_cpuProfilers.emplace(name, new CPUProfilingSession(SAMPLES_AMOUNT)).first->second;
    }
    else
        session = found->second;

    session->OnStartProfiling(m_currentCPUSession, m_cpuOrderCounter++);

    m_currentCPUSession = session;
}

void Profiler::OnEndCPUProfiling(const std::string& name)
{
    ProfilingSession* session = m_cpuProfilers[name];

    assert(m_currentCPUSession == session);

    m_currentCPUSession = m_currentCPUSession->GetParent();
    session->OnEndProfiling();
}

void Profiler::OnStartGPUProfiling(const std::string& name)
{
    auto found = m_gpuProfilers[m_framesCounter % QUERY_LATENCY].find(name);

    ProfilingSession* session;

    if (found == m_gpuProfilers[m_framesCounter % QUERY_LATENCY].end())
    {
        session = m_gpuProfilers[m_framesCounter % QUERY_LATENCY].emplace(name, new GPUProfilingSession(m_d3Device, m_d3Context, SAMPLES_AMOUNT)).first->second;
    }
    else
        session = found->second;

    session->OnStartProfiling(m_currentGPUSession, m_gpuOrderCounter++);

    m_currentGPUSession = session;
}

void Profiler::OnEndGPUProfiling(const std::string& name)
{
    ProfilingSession* session = m_gpuProfilers[m_framesCounter % QUERY_LATENCY][name];

    assert(m_currentGPUSession == session);

    m_currentGPUSession = session->GetParent();
    session->OnEndProfiling();

}

void Profiler::OnDraw()
{
    m_renderingSystem->DrawText(m_cachedMessages, PA_TEXT_SIZE, 0.0f, (float)s_instance->m_window->GetHeight() - 10.0f, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), TextAnchor::Bottom | TextAnchor::Left);
}

void Profiler::OnStartFrame()
{
    m_currentCPUSession = nullptr;
    m_cpuOrderCounter = 0;
    m_gpuOrderCounter = 0;

    ++m_framesCounter;

    m_d3Context->Begin(m_queryJoints[m_framesCounter % QUERY_LATENCY]);
}

void Profiler::OnEndFrame()
{
    m_d3Context->End(m_queryJoints[m_framesCounter % QUERY_LATENCY]);

    m_d3Context->Flush();

    Profiler::StartCPUProfiling("Waiting for GPU");

    while (m_d3Context->GetData(m_queryJoints[(m_framesCounter + 1) % QUERY_LATENCY], NULL, 0, 0) == S_FALSE);

    Profiler::EndCPUProfiling("Waiting for GPU");

    D3D10_QUERY_DATA_TIMESTAMP_DISJOINT tsDisjoint;
    m_d3Context->GetData(m_queryJoints[(m_framesCounter + 1) % QUERY_LATENCY], &tsDisjoint, sizeof(tsDisjoint), 0);
    if (tsDisjoint.Disjoint && m_framesCounter > QUERY_LATENCY - 1)
        DebugLog::LogError("Profiler GPU Query found disjoint!");
    else
    {
        for (auto const& session : m_cpuProfilers)
        {
            session.second->OnEndFrame();
        }

        for (auto const& session : m_gpuProfilers[(m_framesCounter + 1) % QUERY_LATENCY])
        {
            session.second->OnEndFrame();
        }
    }

    m_tmpTime += Time::GetDeltaTime();
    ++m_tmpFramesCounter;

    if (m_tmpTime > 1.0f)
    {
        m_currentFPS = (int)(m_tmpFramesCounter / m_tmpTime);
        m_currentFrameDuration = 1000.0f * m_tmpTime / m_tmpFramesCounter;
        m_currentFrameDuration = (int)(m_currentFrameDuration * 1000) / 1000.0f;
        m_tmpTime = 0.0f;
        m_tmpFramesCounter = 0;
    }

    static stringstream ss;
    ss.str(string());

    ss << "FPS: " << m_currentFPS << " (" << m_currentFrameDuration << "ms)";
    ss << "\n\nCPU PROFILING:";;

    m_cachedMessages = ss.str();

    PrepareLogsHierarchy(m_cpuProfilers.begin(), m_cpuProfilers.end(), (int)m_cpuProfilers.size(), (UINT64)m_CPUfrequency.QuadPart);

    m_profilingTime = 1000.0f * m_profilingTime / m_CPUfrequency.QuadPart;
    m_profilingTime = (int)(m_profilingTime * 1000) / 1000.0f;

    ss.str(string());
    ss << "\nProfiling time:" << m_profilingTime << "ms";
    m_profilingTime = 0.0f;

    ss << "\n\nGPU PROFILING:";
    m_cachedMessages += ss.str();

    PrepareLogsHierarchy(m_gpuProfilers[(m_framesCounter + 1) % QUERY_LATENCY].begin(), m_gpuProfilers[(m_framesCounter + 1) % QUERY_LATENCY].end(), (int)m_gpuProfilers[m_framesCounter % QUERY_LATENCY].size(), tsDisjoint.Frequency);
}

void Profiler::PrepareLogsHierarchy(std::unordered_map<std::string, ProfilingSession*>::iterator begin, std::unordered_map<std::string, ProfilingSession*>::iterator end, const int& length, const UINT64& freq)
{
    std::string* arr = new std::string[length];

    static stringstream ss;

    auto it = begin;
    while (it != end)
    {
        ss.str(string());

        static double result;
        result = it->second->GetAverageResult() / (freq / 1000);
        result = (int)(result * 1000) / 1000.0;

        const ProfilingSession* prof = it->second;
        while ((prof = prof->GetParent()) && prof->GetParent())
        {
            ss << "|        ";
        }

        if (it->second->GetParent())
            ss << "|-----";

        ss << it->first << ": " << result << "ms";

        arr[it->second->GetOrder()] = ss.str();
        ++it;
    }

    ss.str(string());

    for (int i = 0; i < length; ++i)
        ss << "\n" << arr[i];

    m_cachedMessages += ss.str();

    delete[] arr;
}

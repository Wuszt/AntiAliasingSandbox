#include "Profiler.h"
#include "DebugLog.h"
#include "Window.h"
#include <DirectXMath.h>
#include "RenderingSystem.h"
#include <string>
#include "ProfilingSession.h"
#include <d3d11.h>

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
    s_instance->OnStartCPUProfiling(name);
}

void Profiler::EndCPUProfiling(const std::string& name)
{
    s_instance->OnEndCPUProfiling(name);
}

void Profiler::StartGPUProfiling(const std::string& name)
{
    s_instance->OnStartGPUProfiling(name);
}

void Profiler::EndGPUProfiling(const std::string& name)
{
    s_instance->OnEndGPUProfiling(name);
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
    m_currentCPUSession = session->GetParent();
    session->OnEndProfiling();
}

void Profiler::OnStartGPUProfiling(const std::string& name)
{
    auto found = m_gpuProfilers[m_frameCounter % QUERY_LATENCY].find(name);

    ProfilingSession* session;

    if (found == m_gpuProfilers[m_frameCounter % QUERY_LATENCY].end())
    {
        session = m_gpuProfilers[m_frameCounter % QUERY_LATENCY].emplace(name, new GPUProfilingSession(m_d3Device, m_d3Context, SAMPLES_AMOUNT)).first->second;
    }
    else
        session = found->second;

    session->OnStartProfiling(m_currentGPUSession, m_gpuOrderCounter++);

    m_currentGPUSession = session;
}

void Profiler::OnEndGPUProfiling(const std::string& name)
{
    ProfilingSession* session = m_gpuProfilers[m_frameCounter % QUERY_LATENCY][name];
    m_currentGPUSession = session->GetParent();
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
    m_cpuOrderCounter = 0;
    m_gpuOrderCounter = 0;

    ++m_frameCounter;

    m_d3Context->Begin(m_queryJoints[m_frameCounter % QUERY_LATENCY]);
}

void Profiler::OnEndFrame()
{
    m_d3Context->End(m_queryJoints[m_frameCounter % QUERY_LATENCY]);

    Profiler::StartCPUProfiling("Waiting for GPU");

    while (m_d3Context->GetData(m_queryJoints[(m_frameCounter + 1) % QUERY_LATENCY], NULL, 0, 0) == S_FALSE);

    Profiler::EndCPUProfiling("Waiting for GPU");

    D3D10_QUERY_DATA_TIMESTAMP_DISJOINT tsDisjoint;
    m_d3Context->GetData(m_queryJoints[(m_frameCounter + 1) % QUERY_LATENCY], &tsDisjoint, sizeof(tsDisjoint), 0);
    if (tsDisjoint.Disjoint && m_frameCounter > QUERY_LATENCY-1)
        DebugLog::LogError("Profiler GPU Query found disjoint!");
    else
    {
        for (auto const& session : m_cpuProfilers)
        {
            session.second->OnEndFrame();
        }

        for (auto const& session : m_gpuProfilers[(m_frameCounter + 1) % QUERY_LATENCY])
        {
            session.second->OnEndFrame();
        }
    }

    m_cachedMessages.resize(m_cpuProfilers.size() + m_gpuProfilers[m_frameCounter % QUERY_LATENCY].size());

    PrepareLogsHierarchy(m_cpuProfilers.begin(), m_cpuProfilers.end(), (UINT64)m_CPUfrequency.QuadPart, 0);

    PrepareLogsHierarchy(m_gpuProfilers[(m_frameCounter + 1) % QUERY_LATENCY].begin(), m_gpuProfilers[(m_frameCounter + 1) % QUERY_LATENCY].end(), tsDisjoint.Frequency, (int)m_cpuProfilers.size());
}

void Profiler::PrepareLogsHierarchy(std::unordered_map<std::string, ProfilingSession*>::iterator begin, std::unordered_map<std::string, ProfilingSession*>::iterator end, const UINT64& freq, const int& offset)
{
    auto it = begin;
    while (it != end)
    {
        static double result;
        result = it->second->GetAverageResult() / (freq / 1000);

        string spaces = "";
        const ProfilingSession* prof = it->second;
        while ((prof = prof->GetParent()) && prof->GetParent())
        {
            spaces += "|        ";
        }

        if (it->second->GetParent())
            spaces += "|-----";

        m_cachedMessages[offset + it->second->GetOrder()] = spaces + it->first + ": " + to_string(result) + "ms";
        ++it;
    }
}

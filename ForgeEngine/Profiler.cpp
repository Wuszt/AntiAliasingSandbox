#include "Profiler.h"
#include "DebugLog.h"
#include "Window.h"
#include <DirectXMath.h>
#include "UIRenderingSystem.h"
#include <string>
#include "ProfilingSession.h"
#include <d3d11.h>
#include <DirectXCommonClasses/Time.h>
#include <sstream>
#include "Core.h"
#include <iostream>
#include <fstream>

using namespace DirectX;
using namespace std;

Profiler::Profiler(const Window* const& window)
{
    m_window = window;
    QueryPerformanceFrequency(&m_CPUfrequency);

    D3D11_QUERY_DESC desc;
    desc.MiscFlags = 0;
    desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;

    for (int i = 0; i < QUERY_LATENCY; ++i)
        Core::GetD3Device()->CreateQuery(&desc, &m_queryJoints[i]);
}

Profiler::~Profiler()
{
}

void Profiler::Initialize(const Window* const& window)
{
    s_instance = new Profiler(window);
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
    if (s_instance->m_resetRequested)
        s_instance->OnReset();

    static LARGE_INTEGER start, end;
    QueryPerformanceCounter(&start);

    s_instance->OnStartFrame();

    QueryPerformanceCounter(&end);
    s_instance->m_profilingTime += (double)(end.QuadPart - start.QuadPart);
}

void Profiler::EndFrame()
{
    s_instance->OnEndFrame();
}

void Profiler::Reset()
{
    s_instance->m_resetRequested = true;
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
        session = m_gpuProfilers[m_framesCounter % QUERY_LATENCY].emplace(name, new GPUProfilingSession(SAMPLES_AMOUNT)).first->second;
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
    Core::GetUIRenderingSystem()->Print(m_cachedScreenLogs, PA_TEXT_SIZE, 0.0f, (float)s_instance->m_window->GetHeight() - 10.0f, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), TextAnchor::Bottom | TextAnchor::Left);
}

void Profiler::OnStartFrame()
{
    m_currentCPUSession = nullptr;
    m_cpuOrderCounter = 0;
    m_gpuOrderCounter = 0;

    ++m_framesCounter;

    Core::GetD3DeviceContext()->Begin(m_queryJoints[m_framesCounter % QUERY_LATENCY]);
}

void Profiler::OnEndFrame()
{
    static LARGE_INTEGER start, end;
    QueryPerformanceCounter(&start);

    Core::GetD3DeviceContext()->End(m_queryJoints[m_framesCounter % QUERY_LATENCY]);

    QueryPerformanceCounter(&end);
    m_profilingTime += (double)(end.QuadPart - start.QuadPart);

    Profiler::StartCPUProfiling("Waiting for GPU");

    Core::GetD3DeviceContext()->Flush();

    while (Core::GetD3DeviceContext()->GetData(m_queryJoints[(m_framesCounter + 1) % QUERY_LATENCY], NULL, 0, 0) == S_FALSE);

    Profiler::EndCPUProfiling("Waiting for GPU");

    QueryPerformanceCounter(&start);

    D3D10_QUERY_DATA_TIMESTAMP_DISJOINT tsDisjoint;
    Core::GetD3DeviceContext()->GetData(m_queryJoints[(m_framesCounter + 1) % QUERY_LATENCY], &tsDisjoint, sizeof(tsDisjoint), 0);
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

    if (!m_logsFileName.empty())
    {
        SaveLogsToFile(m_logsFileName, tsDisjoint.Frequency);
        m_logsFileName = "";
    }

    PrepareLogsToPrintOnScreen(tsDisjoint.Frequency);

    QueryPerformanceCounter(&end);
    m_profilingTime += (double)(end.QuadPart - start.QuadPart);
}

void Profiler::OnReset()
{
    m_resetRequested = false;

    m_tmpTime = 0.0f;
    m_tmpFramesCounter = 0;

    for (auto& p : m_cpuProfilers)
    {
        p.second->Reset();
    }

    for (int i = 0; i < QUERY_LATENCY; ++i)
    {
        for (auto& p : m_gpuProfilers[i])
        {
            p.second->Reset();
        }
    }
}

void Profiler::SaveLogsToFile(const std::string& fileName, const UINT64& gpuFreq)
{
    std::string str = Core::GetResultsPath() + "/" + fileName + ".csv";

    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    _splitpath_s(str.c_str(), drive, _MAX_DRIVE, &dir[0], _MAX_DIR, nullptr, 0, nullptr, 0);
    std::string dirStr(dir);
    for (size_t i = dirStr.find('/', 1); i - 1 != std::string::npos; i = dirStr.find('/', i) + 1)
    {
        std::string driveDir = std::string(drive) + dirStr.substr(0, i);
        CreateDirectory(LPCSTR(driveDir.c_str()), NULL);
    }
    CreateDirectory(LPCSTR((std::string(drive) + dirStr).c_str()), NULL);

    std::ofstream outFile(str);

    outFile << "FPS" << "," << m_currentFPS << "\n";
    outFile << "Frame" << "," << m_currentFrameDuration << "\n";

    outFile << GetProfilersInCSVFormat(m_cpuProfilers.begin(), m_cpuProfilers.end(), (int)m_cpuProfilers.size(), (UINT64)m_CPUfrequency.QuadPart);

    m_profilingTime = 1000.0f * m_profilingTime / m_CPUfrequency.QuadPart;
    m_profilingTime = (int)(m_profilingTime * 1000) / 1000.0f;

    outFile << "\nProfiling time," << m_profilingTime;
    m_profilingTime = 0.0f;

    outFile << "\n\nGPU PROFILING:";

    outFile << GetProfilersInCSVFormat(m_gpuProfilers[(m_framesCounter + 1) % QUERY_LATENCY].begin(), m_gpuProfilers[(m_framesCounter + 1) % QUERY_LATENCY].end(), (int)m_gpuProfilers[m_framesCounter % QUERY_LATENCY].size(), gpuFreq);
}

void Profiler::PrepareLogsToPrintOnScreen(const UINT64& gpuFreq)
{
    static stringstream ss;
    ss.str(string());

    ss << "FPS: " << m_currentFPS << " (" << m_currentFrameDuration << "ms)";
    ss << "\n\nCPU PROFILING:";;

    ss << GetProfilersInHierarchy(m_cpuProfilers.begin(), m_cpuProfilers.end(), (int)m_cpuProfilers.size(), (UINT64)m_CPUfrequency.QuadPart);

    m_profilingTime = 1000.0f * m_profilingTime / m_CPUfrequency.QuadPart;
    m_profilingTime = (int)(m_profilingTime * 1000) / 1000.0f;

    ss << "\nProfiling time:" << m_profilingTime << "ms";
    m_profilingTime = 0.0f;

    ss << "\n\nGPU PROFILING:";

    ss << GetProfilersInHierarchy(m_gpuProfilers[(m_framesCounter + 1) % QUERY_LATENCY].begin(), m_gpuProfilers[(m_framesCounter + 1) % QUERY_LATENCY].end(), (int)m_gpuProfilers[m_framesCounter % QUERY_LATENCY].size(), gpuFreq);

    m_cachedScreenLogs = ss.str();
}

std::string Profiler::GetProfilersInHierarchy(std::unordered_map<std::string, ProfilingSession*>::iterator begin, std::unordered_map<std::string, ProfilingSession*>::iterator end, const int& length, const UINT64& freq)
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

    delete[] arr;

    return ss.str();
}

std::string Profiler::GetProfilersInCSVFormat(std::unordered_map<std::string, ProfilingSession*>::iterator begin, std::unordered_map<std::string, ProfilingSession*>::iterator end, const int& length, const UINT64& freq)
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

        ss << it->first << "," << result;

        arr[it->second->GetOrder()] = ss.str();
        ++it;
    }

    ss.str(string());

    for (int i = 0; i < length; ++i)
        ss << "\n" << arr[i];

    delete[] arr;

    return ss.str();
}

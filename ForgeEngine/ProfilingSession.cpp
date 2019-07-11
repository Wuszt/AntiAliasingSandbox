#include "ProfilingSession.h"
#include <d3d11.h>
#include <cassert>
#include "Core.h"

ProfilingSession::ProfilingSession(const int& maxSamples)
{
    m_maxSamplesAmount = maxSamples;
    m_currentResult = 0;
    m_results.reserve(maxSamples);
}

ProfilingSession::~ProfilingSession() {}

double ProfilingSession::GetAverageResult()
{
    double sum = 0.0f;
    for (const double& result : m_results)
    {
        sum += result;
    }

    return sum / m_results.size();
}

void ProfilingSession::SaveResult(const double& result)
{
    if (m_results.size() < m_maxSamplesAmount)
    {
        m_results.push_back(result);
        return;
    }

    m_results[m_currentResult] = result;
    m_currentResult = (m_currentResult + 1) % m_maxSamplesAmount;
}

void ProfilingSession::OnStartProfiling(const ProfilingSession* const& parent, const int& order)
{
    assert(!m_active);
    m_active = true;
    m_parent = parent;
    m_order = order;
}

void CPUProfilingSession::OnStartProfiling(const ProfilingSession* const& parent, const int& order)
{
    ProfilingSession::OnStartProfiling(parent, order);
    QueryPerformanceCounter(&StartTick);
}

void CPUProfilingSession::OnEndProfiling()
{
    ProfilingSession::OnEndProfiling();

    static LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    SaveResult((double)(now.QuadPart - StartTick.QuadPart));
}

GPUProfilingSession::GPUProfilingSession(const int& maxSamples) : ProfilingSession(maxSamples)
{
    D3D11_QUERY_DESC desc;
    desc.MiscFlags = 0;
    desc.Query = D3D11_QUERY_TIMESTAMP;

    Core::GetD3Device()->CreateQuery(&desc, &m_start);
    Core::GetD3Device()->CreateQuery(&desc, &m_end);
}

void GPUProfilingSession::OnStartProfiling(const ProfilingSession* const& parent, const int& order)
{
    ProfilingSession::OnStartProfiling(parent, order);

    Core::GetD3DeviceContext()->End(m_start);
}

void GPUProfilingSession::OnEndProfiling()
{
    ProfilingSession::OnEndProfiling();

    Core::GetD3DeviceContext()->End(m_end);
}

void GPUProfilingSession::OnEndFrame()
{
    static UINT64 start, end;

    Core::GetD3DeviceContext()->GetData(m_start, &start, sizeof(UINT64), 0);
    Core::GetD3DeviceContext()->GetData(m_end, &end, sizeof(UINT64), 0);

    SaveResult((double)(end - start));
}

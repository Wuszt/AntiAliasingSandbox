#include "ProfilingSession.h"

ProfilingSession::ProfilingSession(const int& maxSamples)
{
    m_maxSamplesAmount = maxSamples;
    m_currentResult = 0;
    m_results.reserve(maxSamples);
}

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

//void GPUProfilingSession::OnStartProfiling()
//{
//    throw std::logic_error("The method or operation is not implemented.");
//}
//
//void GPUProfilingSession::OnEndProfiling()
//{
//    throw std::logic_error("The method or operation is not implemented.");
//}
//
//void GPUProfilingSession::OnCollectingData()
//{
//    throw std::logic_error("The method or operation is not implemented.");
//}

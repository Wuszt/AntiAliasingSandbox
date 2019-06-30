#include <string>
#include <vector>
#include <Windows.h>

class ProfilingSession
{
public:
    ProfilingSession(const int& maxSamples);
    virtual ~ProfilingSession() = default;

    double GetAverageResult();
    virtual void OnStartProfiling(const ProfilingSession* const& parent, const int& order) { m_active = true; m_parent = parent; m_order = order; }
    virtual void OnEndProfiling() { m_active = false; }

    inline bool IsActive() { return m_active; }
    inline const ProfilingSession* GetParent() const { return m_parent; }
    inline int GetOrder() { return m_order; }

protected:
    void SaveResult(const double& result);

private:
    int m_currentResult = 0;
    std::vector<double> m_results;
    bool m_active = false;
    int m_maxSamplesAmount;
    const ProfilingSession* m_parent;
    int m_order;
};

class CPUProfilingSession : public ProfilingSession
{
public:
    CPUProfilingSession(const int& maxSamples) : ProfilingSession(maxSamples) {}

    virtual void OnStartProfiling(const ProfilingSession* const& parent, const int& order) override;
    virtual void OnEndProfiling() override;

private:
    LARGE_INTEGER StartTick;
};

//class GPUProfilingSession : public ProfilingSession
//{
//
//public:
//    virtual void OnStartProfiling() override;
//
//
//    virtual void OnEndProfiling() override;
//
//
//    void OnCollectingData();
//
//};
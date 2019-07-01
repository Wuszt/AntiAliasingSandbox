#include <string>
#include <vector>
#include <Windows.h>

struct ID3D11Query;
struct ID3D11DeviceContext;
struct ID3D11Device;

class ProfilingSession
{
public:
    ProfilingSession(const int& maxSamples);
    virtual ~ProfilingSession() = 0;

    double GetAverageResult();
    virtual void OnStartProfiling(const ProfilingSession* const& parent, const int& order);
    virtual void OnEndProfiling() { m_active = false; }

    inline bool IsActive() { return m_active; }
    inline const ProfilingSession* GetParent() const { return m_parent; }
    inline void SetParent(const ProfilingSession* const& session) { m_parent = session; }
    inline int GetOrder() { return m_order; }

    virtual void OnStartFrame() {}
    virtual void OnEndFrame() {}

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
    virtual ~CPUProfilingSession() override {}

    virtual void OnStartProfiling(const ProfilingSession* const& parent, const int& order) override;
    virtual void OnEndProfiling() override;

private:
    LARGE_INTEGER StartTick;
};

class GPUProfilingSession : public ProfilingSession
{

public:
    GPUProfilingSession(ID3D11Device* const& d3Device, ID3D11DeviceContext* const& d3Context, const int& maxSamples);
    virtual ~GPUProfilingSession() override {}

    virtual void OnStartProfiling(const ProfilingSession* const& parent, const int& order) override;
    virtual void OnEndProfiling() override;
    virtual void OnEndFrame() override;

private:
    ID3D11Query* m_start;
    ID3D11Query* m_end;

    ID3D11DeviceContext* m_d3Context;
};
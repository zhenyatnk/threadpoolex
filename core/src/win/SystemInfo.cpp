#include <threadpoolex/core/SystemInfo.hpp>

#ifdef _WIN32

#include "pdh.h"
#include "TCHAR.h"
#include "windows.h"

namespace threadpoolex {
namespace core {
//-----------------------------------------
class CGetterUsageCPU
    :public IUsageCPU
{
public:
    CGetterUsageCPU();

    CGetterUsageCPU(const CGetterUsageCPU&) = delete;
    CGetterUsageCPU& operator=(const CGetterUsageCPU&) = delete;

    virtual float GetUsageCPU() const override;

private:
    PDH_HQUERY m_CpuQuery;
    PDH_HCOUNTER m_CpuTotal;
};

CGetterUsageCPU::CGetterUsageCPU()
{
    PdhOpenQuery(NULL, NULL, &m_CpuQuery);
    PdhAddEnglishCounter(m_CpuQuery, TEXT("\\Processor(_Total)\\% Processor Time"), NULL, &m_CpuTotal);
    PdhCollectQueryData(m_CpuQuery);
}

float CGetterUsageCPU::GetUsageCPU() const
{
    PDH_FMT_COUNTERVALUE lCounterVal;

    PdhCollectQueryData(m_CpuQuery);
    PdhGetFormattedCounterValue(m_CpuTotal, PDH_FMT_DOUBLE, NULL, &lCounterVal);
    return lCounterVal.doubleValue;
}
//-----------------------------------------
class CGetterUsageCPUProccess
    :public IUsageCPUProccess
{
public:
    CGetterUsageCPUProccess();

    CGetterUsageCPUProccess(const CGetterUsageCPUProccess&) = delete;
    CGetterUsageCPUProccess& operator=(const CGetterUsageCPUProccess&) = delete;

    virtual float GetUsageCPUProccess() const override;

private:
    mutable ULARGE_INTEGER m_LastCPU;
    mutable ULARGE_INTEGER m_LastSysCPU;
    mutable ULARGE_INTEGER m_LastUserCPU;
    int m_NumProcessors;
    HANDLE m_Handle;
};

CGetterUsageCPUProccess::CGetterUsageCPUProccess()
{
    SYSTEM_INFO sysInfo;
    FILETIME ftime, fsys, fuser;

    GetSystemInfo(&sysInfo);
    m_NumProcessors = sysInfo.dwNumberOfProcessors;

    GetSystemTimeAsFileTime(&ftime);
    memcpy(&m_LastCPU, &ftime, sizeof(FILETIME));

    m_Handle = GetCurrentProcess();
    GetProcessTimes(m_Handle, &ftime, &ftime, &fsys, &fuser);
    memcpy(&m_LastSysCPU, &fsys, sizeof(FILETIME));
    memcpy(&m_LastUserCPU, &fuser, sizeof(FILETIME));
}

float CGetterUsageCPUProccess::GetUsageCPUProccess() const
{
    FILETIME ftime, fsys, fuser;
    ULARGE_INTEGER now, sys, user;
    double percent;

    GetSystemTimeAsFileTime(&ftime);
    memcpy(&now, &ftime, sizeof(FILETIME));

    GetProcessTimes(m_Handle, &ftime, &ftime, &fsys, &fuser);
    memcpy(&sys, &fsys, sizeof(FILETIME));
    memcpy(&user, &fuser, sizeof(FILETIME));
    percent = (sys.QuadPart - m_LastSysCPU.QuadPart) +
        (user.QuadPart - m_LastUserCPU.QuadPart);
    percent /= (now.QuadPart - m_LastCPU.QuadPart);
    percent /= m_NumProcessors;
    m_LastCPU = now;
    m_LastUserCPU = user;
    m_LastSysCPU = sys;

    return percent * 100;
}
//-----------------------------------------
class CSystemInfo
    :public ISystemInfo
{
public:
    virtual float GetUsageCPU() const override;
    virtual float GetUsageCPUProccess() const override;

protected:
    IUsageCPU::Ptr GetGetterUsageCPU() const;
    IUsageCPUProccess::Ptr GetGetterUsageCPUProccess() const;

private:
    mutable IUsageCPU::Ptr m_CPU;
    mutable IUsageCPUProccess::Ptr m_CPUByProccess;
};

IUsageCPU::Ptr CSystemInfo::GetGetterUsageCPU() const
{
    if (!m_CPU)
        m_CPU = std::make_shared<CGetterUsageCPU>();
    return m_CPU;
}

IUsageCPUProccess::Ptr CSystemInfo::GetGetterUsageCPUProccess() const
{
    if (!m_CPUByProccess)
        m_CPUByProccess = std::make_shared<CGetterUsageCPUProccess>();
    return m_CPUByProccess;
}

float CSystemInfo::GetUsageCPU() const
{
    return GetGetterUsageCPU()->GetUsageCPU();
}

float CSystemInfo::GetUsageCPUProccess() const
{
    return GetGetterUsageCPUProccess()->GetUsageCPUProccess();
}

ISystemInfo::Ptr CreateSystemInfo()
{
    return std::make_shared<CSystemInfo>();
}

}
}

#endif
#include <threadpoolex/core/IThreadPool.hpp>
#include <baseex/core/ISystemInfo.hpp>
#include <baseex/core/ITimerActive.hpp>

#include <stdio.h>
#include <iostream>

using namespace baseex::core;

namespace
{
class CObserverPrintCPU
    :public baseex::core::EmptyObserverTimer
{
public:
    CObserverPrintCPU(baseex::core::ISystemInfo::Ptr aSysInfo)
        :m_SysInfo(aSysInfo)
    {}

    void HandleCheck() override
    {
        std::cout <<"UsageCPU: "<< m_SysInfo->GetUsageCPU() <<". UsageCPUProccess:" << m_SysInfo->GetUsageCPUProccess() << std::endl;
    }
    void HandleClose() override
    {
        std::cout << "UsageCPU: " << m_SysInfo->GetUsageCPU() << ". UsageCPUProccess:" << m_SysInfo->GetUsageCPUProccess() << std::endl;
    }

private:
    ISystemInfo::Ptr m_SysInfo;
};
}

int main(int ac, char** av)
{
    ITimerActive::Ptr lTimer = CreateTimerActive(1000);
    lTimer->AddObserver(std::make_shared<CObserverPrintCPU>(CreateSystemInfo()));

    while (true);

    getchar();
    return 0;
}
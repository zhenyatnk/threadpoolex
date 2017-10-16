#include <threadpoolex/core/IThreadPool.hpp>
#include <threadpoolex/core/ISystemInfo.hpp>
#include <threadpoolex/core/ITimerActive.hpp>

#include <stdio.h>
#include <iostream>

using namespace threadpoolex::core;

namespace
{
class CObserverPrintCPU
    :public EmptyObserverTimer
{
public:
    CObserverPrintCPU(ISystemInfo::Ptr aSysInfo)
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
    threadpoolex::core::ITimerActive::Ptr lTimer = CreateTimerActive(1000);
    lTimer->AddObserver(std::make_shared<CObserverPrintCPU>(CreateSystemInfo()));

    while (true);

    getchar();
    return 0;
}
#include <threadpoolex/core/IThreadPool.hpp>
#include <baseex/core/ISystemInfo.hpp>
#include <baseex/core/ITimerActive.hpp>

#include <stdio.h>
#include <iostream>

using namespace baseex::core;

namespace
{
class CObserverPrintUsageCPUProccess
    :public baseex::core::EmptyObserverTimer
{
public:
    CObserverPrintUsageCPUProccess(baseex::core::ISystemInfo::Ptr aSysInfo)
        :m_SysInfo(aSysInfo)
    {}

    void HandleCheck() override
    {
        std::cout << "UsageCPUProccess:" << m_SysInfo->GetUsageCPUProccess() << std::endl;
    }
    void HandleClose() override
    {
        std::cout << "UsageCPUProccess:" << m_SysInfo->GetUsageCPUProccess() << std::endl;
    }
    virtual void HandleError(const std::string &aMessage, const int& aErrorCode) override
    {
        std::cout << aMessage << std::endl;
    }

private:
    ISystemInfo::Ptr m_SysInfo;
};
    
class CObserverPrintUsageCPU
:public baseex::core::EmptyObserverTimer
{
public:
    CObserverPrintUsageCPU(baseex::core::ISystemInfo::Ptr aSysInfo)
        :m_SysInfo(aSysInfo)
    {}
    
    void HandleCheck() override
    {
        std::cout <<"UsageCPU: "<< m_SysInfo->GetUsageCPU() << std::endl;
    }
    void HandleClose() override
    {
        std::cout << "UsageCPU: " << m_SysInfo->GetUsageCPU() << std::endl;
    }
    virtual void HandleError(const std::string &aMessage, const int& aErrorCode) override
    {
        std::cout << aMessage << std::endl;
    }
    
private:
    ISystemInfo::Ptr m_SysInfo;
};
}

int main(int ac, char** av)
{
    ITimerActive::Ptr lTimer = CreateTimerActive(1000);
    auto sysinfo = CreateSystemInfo();
    lTimer->AddObserver(std::make_shared<CObserverPrintUsageCPUProccess>(sysinfo));
    lTimer->AddObserver(std::make_shared<CObserverPrintUsageCPU>(sysinfo));
    
    while (true);

    getchar();
    return 0;
}

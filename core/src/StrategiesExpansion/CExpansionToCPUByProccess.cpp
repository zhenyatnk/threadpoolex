#include <threadpoolex/core/IStrategyExpansion.hpp>

namespace threadpoolex {
namespace core {

//---------------------------------------------------------------
class CExpansionToCPUByProccess
    :public IStrategyExpansion
{
public:
    explicit CExpansionToCPUByProccess(IUsageCPUProccess::Ptr aUsageCPUProccess, uint8_t aMaxUsageCPU, unsigned int aCountMinThread);

    virtual int8_t GetOptimalDiffWorkers(unsigned int aCountUnworkingTasks, unsigned int aCountExistsThreads) const override;

private:
    IUsageCPUProccess::Ptr m_UsageCPUProccess;
    uint8_t m_MaxUsageCPU;
    unsigned int m_CountMinThread;
};

CExpansionToCPUByProccess::CExpansionToCPUByProccess(IUsageCPUProccess::Ptr aUsageCPUProccess, uint8_t aMaxUsageCPU, unsigned int aCountMinThread)
    :m_UsageCPUProccess(aUsageCPUProccess), m_MaxUsageCPU(aMaxUsageCPU), m_CountMinThread(aCountMinThread)
{}

int8_t CExpansionToCPUByProccess::GetOptimalDiffWorkers(unsigned int aCountUnworkingTasks, unsigned int aCountExistsThreads) const
{
    int8_t lDiff = 0;

    auto lCurrentCPU = m_UsageCPUProccess->GetUsageCPUProccess();
    if (lCurrentCPU < m_MaxUsageCPU)
        lDiff = 1;
    else if (lCurrentCPU > m_MaxUsageCPU + m_MaxUsageCPU / 10)
        lDiff = -1;

    if ((!aCountUnworkingTasks && lDiff == 1) ||
        (aCountExistsThreads <= m_CountMinThread && lDiff == -1))
        lDiff = 0;

    return lDiff;
}

IStrategyExpansion::Ptr CreateExpansionToCPUByProccess(IUsageCPUProccess::Ptr aUsageCPUProccess, uint8_t aMaxUsageCPU, unsigned int aCountMinThread)
{
    return std::make_shared<CExpansionToCPUByProccess>(aUsageCPUProccess, aMaxUsageCPU, aCountMinThread);
}

}
}

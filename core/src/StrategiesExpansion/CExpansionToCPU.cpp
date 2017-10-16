#include <threadpoolex/core/IStrategyExpansion.hpp>

namespace threadpoolex {
namespace core {

//---------------------------------------------------------------
class CExpansionToCPU
    :public IStrategyExpansion
{
public:
    explicit CExpansionToCPU(IUsageCPU::Ptr aUsageCPU, uint8_t aMaxUsageCPU, unsigned int aCountMinThread);

    virtual int8_t GetOptimalDiffWorkers(unsigned int aCountUnworkingTasks, unsigned int aCountExistsThreads) const override;

private:
    IUsageCPU::Ptr m_UsageCPU;
    uint8_t m_MaxUsageCPU;
    unsigned int m_CountMinThread;
};

CExpansionToCPU::CExpansionToCPU(IUsageCPU::Ptr aUsageCPU, uint8_t aMaxUsageCPU, unsigned int aCountMinThread)
    :m_UsageCPU(aUsageCPU), m_MaxUsageCPU(aMaxUsageCPU), m_CountMinThread(aCountMinThread)
{}

int8_t CExpansionToCPU::GetOptimalDiffWorkers(unsigned int aCountUnworkingTasks, unsigned int aCountExistsThreads) const
{
    int8_t lDiff = 0;

    auto lCurrentCPU = m_UsageCPU->GetUsageCPU();
    if (lCurrentCPU < m_MaxUsageCPU)
        lDiff = 1;
    else if (lCurrentCPU > m_MaxUsageCPU + m_MaxUsageCPU / 10)
        lDiff = -1;

    if ((!aCountUnworkingTasks && lDiff == 1) ||
        (aCountExistsThreads <= m_CountMinThread && lDiff == -1))
        lDiff = 0;

    return lDiff;
}

IStrategyExpansion::Ptr CreateExpansionToCPU(IUsageCPU::Ptr aUsageCPU, uint8_t aMaxUsageCPU, unsigned int aCountMinThread)
{
    return std::make_shared<CExpansionToCPU>(aUsageCPU, aMaxUsageCPU, aCountMinThread);
}

}
}

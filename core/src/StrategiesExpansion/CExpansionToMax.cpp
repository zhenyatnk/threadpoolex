#include <threadpoolex/core/IStrategyExpansion.hpp>

namespace threadpoolex {
namespace core {

//---------------------------------------------------------------
class CExpansionToMax
    :public IStrategyExpansion
{
public:
    explicit CExpansionToMax(unsigned int aCountMaxThread);

    virtual int8_t GetOptimalDiffWorkers(unsigned int aCountUnworkingTasks, unsigned int aCountExistsThreads) const override;

private:
    unsigned int m_CountMaxThread;
};

CExpansionToMax::CExpansionToMax(unsigned int aCountMaxThread)
    :m_CountMaxThread(aCountMaxThread)
{}

int8_t CExpansionToMax::GetOptimalDiffWorkers(unsigned int aCountUnworkingTasks, unsigned int aCountExistsThreads) const
{

    return (!!aCountUnworkingTasks && m_CountMaxThread > aCountExistsThreads) ? 1 : 0 ;
}

IStrategyExpansion::Ptr CreateExpansionToMax(unsigned int aCountMaxThread)
{
    return std::make_shared<CExpansionToMax>(aCountMaxThread);
}

}
}
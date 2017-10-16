#pragma once

#include <threadpoolex/core/ISystemInfo.hpp>

#include <memory>

namespace threadpoolex {
namespace core {

class IStrategyExpansion
{
public:
    using Ptr = std::shared_ptr<IStrategyExpansion>;

public:
    virtual ~IStrategyExpansion() = default;

    virtual int8_t GetOptimalDiffWorkers(unsigned int aCountUnworkingTasks, unsigned int aCountExistsThreads) const = 0;
};

IStrategyExpansion::Ptr CreateExpansionToMax(unsigned int aCountMaxThread);
IStrategyExpansion::Ptr CreateExpansionToCPU(IUsageCPU::Ptr aUsageCPU, uint8_t aMaxUsageCPU, unsigned int aCountMinThread);
IStrategyExpansion::Ptr CreateExpansionToCPUByProccess(IUsageCPUProccess::Ptr aUsageCPUProccess, uint8_t aMaxUsageCPU, unsigned int aCountMinThread);

}
}
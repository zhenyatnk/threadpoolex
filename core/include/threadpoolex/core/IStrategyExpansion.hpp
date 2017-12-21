#pragma once

#include <threadpoolex/core/export.hpp>
#include <baseex/core/ISystemInfo.hpp>

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

THREADPOOLEX_CORE_EXPORT IStrategyExpansion::Ptr CreateExpansionToMax(unsigned int aCountMaxThread);
THREADPOOLEX_CORE_EXPORT IStrategyExpansion::Ptr CreateExpansionToCPU(baseex::core::IUsageCPU::Ptr aUsageCPU, uint8_t aMaxUsageCPU, unsigned int aCountMinThread);
THREADPOOLEX_CORE_EXPORT IStrategyExpansion::Ptr CreateExpansionToCPUByProccess(baseex::core::IUsageCPUProccess::Ptr aUsageCPUProccess, uint8_t aMaxUsageCPU, unsigned int aCountMinThread);

}
}
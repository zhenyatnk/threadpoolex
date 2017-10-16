#pragma once

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

}
}
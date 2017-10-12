#pragma once

#include <threadpoolex/core/ITaskObserver.hpp>

#include <memory>

namespace threadpoolex {
namespace core {

class ITask
    :public virtual IObservableTask
{
public:
    using Ptr = std::shared_ptr<ITask>;

public:
    virtual ~ITask() = default;

    virtual void Execute() = 0;
};

}
}
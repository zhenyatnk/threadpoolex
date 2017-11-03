#pragma once

#include <threadpoolex/core/ITask.hpp>

namespace threadpoolex {
namespace core {

class IWait
{
public:
    using Ptr = std::shared_ptr<IWait>;

public:
    virtual void Wait() = 0;
};

THREADPOOLEX_CORE_EXPORT ITask::Ptr CreateTaskWait(ITask::Ptr aTask, IWait::Ptr &aWaiter);

}
}

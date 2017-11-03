#pragma once

#include <threadpoolex/core/ITask.hpp>

namespace threadpoolex {
namespace core {

class ITaskWait
    :public ITask
{
public:
    using Ptr = std::shared_ptr<ITaskWait>;

public:
    virtual void Wait() = 0;
};

THREADPOOLEX_CORE_EXPORT ITaskWait::Ptr CreateTaskWait(ITask::Ptr aTask);

}
}

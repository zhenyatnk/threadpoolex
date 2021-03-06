#pragma once

#include <threadpoolex/core/ITask.hpp>
#include <threadpoolex/core/IStrategyExpansion.hpp>
#include <threadpoolex/version/version.hpp>

#include <memory>
#include <vector>

namespace threadpoolex {
namespace core {

//--------------------------------------------------------------
class IThreadPool
{
public:
    using Ptr = std::shared_ptr<IThreadPool>;
    using WPtr = std::weak_ptr<IThreadPool>;    

public:
    virtual ~IThreadPool() = default;

    virtual void AddTask(ITask::Ptr) = 0;
    virtual void AddTasks(const std::vector<ITask::Ptr>&) = 0;
    virtual void AddTaskToTop(ITask::Ptr) = 0;

    virtual void TryExpansion() = 0;
};
//--------------------------------------------------------------
THREADPOOLEX_CORE_EXPORT IThreadPool::Ptr CreateThreadPool(unsigned int aCountStartThreads, IStrategyExpansion::Ptr aExpansion);

}
}

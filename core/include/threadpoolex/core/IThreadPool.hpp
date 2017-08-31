#pragma once

#include <threadpoolex/core/ITask.hpp>
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

public:
    virtual ~IThreadPool() = default;

    virtual void AddTask(ITask::Ptr) = 0;
    virtual void AddTaskToTop(ITask::Ptr) = 0;
};
//--------------------------------------------------------------
IThreadPool::Ptr CreateThreadPool(unsigned int aNumberThreads);
//--------------------------------------------------------------
}
}
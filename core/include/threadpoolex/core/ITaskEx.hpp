#pragma once

#include <threadpoolex/core/ITask.hpp>

#include <memory>
#include <chrono>

namespace threadpoolex {
namespace core {

class ITaskEx
   :public ITask
{
public:
    using Ptr = std::shared_ptr<ITaskEx>;
    using TimeType = std::chrono::steady_clock::time_point;

public:
    virtual ~ITaskEx() = default;

    virtual TimeType GetTimeStart() const = 0;
    virtual TimeType GetTimeEnd() const = 0;
    virtual bool IsProcessing() const = 0;
};

ITaskEx::Ptr CreateTaskEx(ITask::Ptr aTask);

}
}

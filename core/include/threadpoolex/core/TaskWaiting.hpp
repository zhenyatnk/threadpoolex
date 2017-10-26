#pragma once

#include <threadpoolex/core/ITask.hpp>

#include <future>

namespace threadpoolex {
namespace core {

THREADPOOLEX_CORE_EXPORT ITask::Ptr CreateWaitingTask(ITask::Ptr aTask, std::promise<void> &&aPromise);

}
}

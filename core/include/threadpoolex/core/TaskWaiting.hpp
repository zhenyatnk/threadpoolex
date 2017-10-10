#pragma once

#include <threadpoolex/core/ITask.hpp>

#include <future>

namespace threadpoolex {
namespace core {

ITask::Ptr CreateWaitingTask(ITask::Ptr aTask, std::promise<void> &&aPromise);

}
}

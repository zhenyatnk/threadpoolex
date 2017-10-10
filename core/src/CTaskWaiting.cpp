#include <threadpoolex/core/TaskWaiting.hpp>

namespace threadpoolex {
namespace core {

class CTaskWaiting
    :public ITask
{
public:
    CTaskWaiting(ITask::Ptr aTask, std::promise<void> &&aPromise);

    virtual void Execute() override;

private:
    ITask::Ptr m_Task;
    std::promise<void> m_Promise;
};

CTaskWaiting::CTaskWaiting(ITask::Ptr aTask, std::promise<void> &&aPromise)
    :m_Task(aTask), m_Promise(std::move(aPromise))
{}

void CTaskWaiting::Execute()
{
    m_Task->Execute();
    m_Promise.set_value();
}

ITask::Ptr CreateWaitingTask(ITask::Ptr aTask, std::promise<void> &&aPromise)
{
    return std::make_shared<CTaskWaiting>(aTask, std::move(aPromise));
}

}
}

#include <threadpoolex/core/RAII.hpp>
#include <threadpoolex/core/ITaskExceptions.hpp>
#include <threadpoolex/core/TaskWaiting.hpp>

namespace threadpoolex {
namespace core {

class CTaskWaiting
    :public ITask, virtual CBaseObservableTask
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
    try
    {
        CRAII<CObservableTask::Ptr> l(this->GetObserver(), [](CObservableTask::Ptr aObserver) { aObserver->NotifyStart(); },
                                                           [](CObservableTask::Ptr aObserver) { aObserver->NotifyComplete(); });
        m_Task->Execute();
        m_Promise.set_value();

    }
    CATCH_CODE_ERROR(exceptions_base::error_base, this->GetObserver()->NotifyError);
}

ITask::Ptr CreateWaitingTask(ITask::Ptr aTask, std::promise<void> &&aPromise)
{
    return std::make_shared<CTaskWaiting>(aTask, std::move(aPromise));
}

}
}

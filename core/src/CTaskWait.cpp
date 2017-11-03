#include <threadpoolex/core/RAII.hpp>
#include <threadpoolex/core/ITaskExceptions.hpp>
#include <threadpoolex/core/ITaskWait.hpp>
#include <threadpoolex/core/TaskPromise.hpp>

#include <future>

namespace threadpoolex {
namespace core {

//-------------------------------------------------------
class CTaskWait
    :public ITaskWait, virtual CBaseObservableTask
{
public:
    CTaskWait(ITask::Ptr aTask);

    virtual void Execute() override;
    virtual void Wait() override;

private:
    ITask::Ptr m_Task;
    std::future<void> m_Future;
};

CTaskWait::CTaskWait(ITask::Ptr aTask)
{
    std::promise<void> promise;
    m_Future = promise.get_future();
    m_Task = CreateTaskPromise(aTask, std::move(promise));
}

void CTaskWait::Execute()
{
    try
    {
        CRAII<CObservableTask::Ptr> l(this->GetObserver(), [](CObservableTask::Ptr aObserver) { aObserver->NotifyStart(); },
            [](CObservableTask::Ptr aObserver) { aObserver->NotifyComplete(); });
        m_Task->Execute();
    }
    catch (exceptions_base::error_base &aErr)
    {
        this->GetObserver()->NotifyError(aErr.what(), aErr.GetErrorCode());
    }
}

void CTaskWait::Wait()
{
    if(m_Future.valid())
        m_Future.wait();
}
//-------------------------------------------------------

ITaskWait::Ptr CreateTaskWait(ITask::Ptr aTask)
{
    return std::make_shared<CTaskWait>(aTask);
}


}
}

#include <threadpoolex/core/RAII.hpp>
#include <threadpoolex/core/ITaskExceptions.hpp>
#include <threadpoolex/core/ITaskWait.hpp>

#include <future>

namespace threadpoolex {
namespace core {

class CWaitFuture
    :public IWait
{
public:
    CWaitFuture(std::future<void> &&aFuture);
    virtual void Wait() override;

private:
    std::future<void> m_Future;
};

CWaitFuture::CWaitFuture(std::future<void> &&aFuture)
    :m_Future(std::move(aFuture))
{}

void CWaitFuture::Wait()
{
    if (m_Future.valid())
        m_Future.get();
}
//-----------------------------------------------------------
class CTaskWait
    :public ITask, virtual CBaseObservableTask
{
public:
    CTaskWait(ITask::Ptr aTask, IWait::Ptr &aWaiter);

    virtual void Execute() override;

private:
    ITask::Ptr m_Task;
    std::promise<void> m_Promise;
};

CTaskWait::CTaskWait(ITask::Ptr aTask, IWait::Ptr &aWaiter)
    :m_Task(aTask)
{
    aWaiter = std::make_shared<CWaitFuture>(m_Promise.get_future());
}

void CTaskWait::Execute()
{
    try
    {
        CRAII<CObservableTask::Ptr> l(this->GetObserver(), [](CObservableTask::Ptr aObserver) { aObserver->NotifyStart(); },
            [](CObservableTask::Ptr aObserver) { aObserver->NotifyComplete(); });
        m_Task->Execute();
        m_Promise.set_value();
    }
    catch (exceptions_base::error_base &aErr)
    {
        this->GetObserver()->NotifyError(aErr.what(), aErr.GetErrorCode());
        m_Promise.set_exception(std::current_exception());
    }
    catch (...)
    {
        m_Promise.set_exception(std::current_exception());
    }
}
//-------------------------------------------------------

ITask::Ptr CreateTaskWait(ITask::Ptr aTask, IWait::Ptr &aWaiter)
{
    return std::make_shared<CTaskWait>(aTask, aWaiter);
}

}
}

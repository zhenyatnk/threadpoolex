#include <threadpoolex/core/ITaskEx.hpp>
#include <threadpoolex/core/ITaskExceptions.hpp>
#include <baseex/core/RAII.hpp>

#include <mutex>

namespace threadpoolex {
namespace core {

class CTaskEx
   :public ITaskEx, virtual CBaseObservableTask
{
public:
    explicit CTaskEx(ITask::Ptr aTask);

    virtual ITaskEx::TimeType GetTimeStart() const override;
    virtual ITaskEx::TimeType GetTimeEnd() const override;
    virtual bool IsProcessing() const override;

    virtual void Execute() override;

private:
     ITask::Ptr m_Task;
     ITaskEx::TimeType m_Start;
     ITaskEx::TimeType m_End;
     std::atomic_bool m_IsProcessing;
     std::mutex m_TimeMutex;
};

CTaskEx::CTaskEx(ITask::Ptr aTask)
    :m_Task(aTask), m_IsProcessing(false)
{}

ITaskEx::TimeType CTaskEx::GetTimeStart() const
{
    return m_Start;
}

ITaskEx::TimeType CTaskEx::GetTimeEnd() const
{
    return m_End;
}

bool CTaskEx::IsProcessing() const
{
    return m_IsProcessing;
}

void CTaskEx::Execute()
{
    try
    {
        baseex::core::CRAII<CObservableTask::Ptr> l(this->GetObserver(), [](CObservableTask::Ptr aObserver) { aObserver->NotifyStart(); },
            [](CObservableTask::Ptr aObserver) { aObserver->NotifyComplete(); });
        baseex::core::CRAII<void> lTime(
            [this]() { baseex::core::lock_guard_ex<std::mutex> lLock(m_TimeMutex); m_Start = std::chrono::steady_clock::now(); },
            [this]() { baseex::core::lock_guard_ex<std::mutex> lLock(m_TimeMutex); m_End = std::chrono::steady_clock::now(); });
        baseex::core::set_values_raii<decltype(m_IsProcessing)> lProcessing(m_IsProcessing, true, false);
        m_Task->Execute();
    }
    CATCH_CODE_ERROR(baseex::core::exceptions_base::error_base, this->GetObserver()->NotifyError);
}

ITaskEx::Ptr CreateTaskEx(ITask::Ptr aTask)
{
    return std::make_shared<CTaskEx>(aTask);
}
}
}

#include <threadpoolex/core/IThreadPool.hpp>
#include <baseex/core/TLockingForce.hpp>
#include <baseex/core/TNotifier.hpp>
#include <baseex/core/RAII.hpp>

#include <vector>
#include <deque>
#include <future>

namespace threadpoolex {
namespace core {
namespace {
//---------------------------------------------------------------

using TDequeTasks = baseex::core::TLockingEx<std::deque<ITask::Ptr>>;

//---------------------------------------------------------------
class IThread
{
public:
    using Ptr = std::shared_ptr<IThread>;

public:
    virtual ~IThread() = default;

    virtual void SetDequeTasksNonLock(std::shared_ptr<TDequeTasks> aTasks) = 0;
};

}
//---------------------------------------------------------------
class CWorker
    :public IThread
{
public:
    explicit CWorker(std::shared_ptr<TDequeTasks> aTasks);
    virtual ~CWorker();

    virtual void SetDequeTasksNonLock(std::shared_ptr<TDequeTasks> aTasks) override;

protected:
    void Run();

private:
    std::shared_ptr<TDequeTasks> m_Tasks;
    baseex::core::thread_join_raii m_ThreadRaii;
    std::atomic_bool m_Stop;
};

CWorker::CWorker(std::shared_ptr<TDequeTasks> aTasks)
    :m_Tasks(aTasks), m_Stop(false)
{
    Run();
}

CWorker::~CWorker()
{
    m_Stop = true;
    m_ThreadRaii = baseex::core::thread_join_raii();
}

void CWorker::SetDequeTasksNonLock(std::shared_ptr<TDequeTasks> aTasks)
{
    m_Tasks = aTasks;
}

void CWorker::Run()
{
    m_ThreadRaii = baseex::core::thread_join_raii(std::thread(
    [this]()
    {
        while (true)
        {
            if (m_Stop)
                break;

            while (m_Tasks->empty() && !m_Stop)
                std::this_thread::sleep_for(std::chrono::milliseconds(200));

            if (m_Stop)
                break;

            {
                ITask::Ptr lTask;
                {
                    baseex::core::lock_guard_ex<std::shared_ptr<TDequeTasks>> lock(m_Tasks);

                    if (!m_Tasks->empty())
                    {
                        lTask = m_Tasks->front();
                        m_Tasks->pop_front();
                    }
                }

                if (lTask)
                    lTask->Execute();
            }
        }
    }));
}
//------------------------------------------------------------------

class CThreadPool
    :public IThreadPool
{
public:
    CThreadPool(unsigned int aCountStartThreads, IStrategyExpansion::Ptr aExpansion);

    virtual void AddTask(ITask::Ptr) override;
    virtual void AddTasks(const std::vector<ITask::Ptr>&) override;
    virtual void AddTaskToTop(ITask::Ptr) override;

    virtual void TryExpansion() override;

protected:
    void TryExpansionNonLock();
    void ParkingWorkers(uint8_t aCount);
    void AddWorkers(uint8_t aCount);

private:
    std::vector<IThread::Ptr> m_Threads;
    std::vector<IThread::Ptr> m_ParkingThreads;
    std::shared_ptr<TDequeTasks> m_Tasks;
    IStrategyExpansion::Ptr m_Expansion;
};

CThreadPool::CThreadPool(unsigned int aCountStartThreads, IStrategyExpansion::Ptr aExpansion)
    :m_Tasks(std::make_shared<TDequeTasks>()), m_Expansion(aExpansion)
{
    for (unsigned int i = 0; i < aCountStartThreads; i++)
        m_Threads.push_back(std::make_shared<CWorker>(m_Tasks));
}

void CThreadPool::AddTask(ITask::Ptr aTask)
{
    baseex::core::force_lock_guard_ex<std::shared_ptr<TDequeTasks>> lock(m_Tasks);
    m_Tasks->push_back(aTask);
}

void CThreadPool::AddTasks(const std::vector<ITask::Ptr>& aTasks)
{
    baseex::core::force_lock_guard_ex<std::shared_ptr<TDequeTasks>> lock(m_Tasks);
    m_Tasks->insert(m_Tasks->end(), aTasks.begin(), aTasks.end());
}

void CThreadPool::AddTaskToTop(ITask::Ptr aTask)
{
    baseex::core::force_lock_guard_ex<std::shared_ptr<TDequeTasks>> lock(m_Tasks);
    m_Tasks->push_front(aTask);
}

void CThreadPool::TryExpansion()
{
    baseex::core::force_lock_guard_ex<std::shared_ptr<TDequeTasks>> lock(m_Tasks);
    TryExpansionNonLock();
}

void CThreadPool::TryExpansionNonLock()
{
    auto lDiff = m_Expansion->GetOptimalDiffWorkers(m_Tasks->size(), m_Threads.size());

    if      (lDiff > 0)  AddWorkers(lDiff);
    else if (lDiff < 0)  ParkingWorkers(abs(lDiff));
}

void CThreadPool::ParkingWorkers(uint8_t aCount)
{
    while (!!aCount-- && !m_Threads.empty())
    {
        auto lWorker = m_Threads.back();
        m_Threads.pop_back();
        lWorker->SetDequeTasksNonLock(std::make_shared<TDequeTasks>());
        m_ParkingThreads.push_back(lWorker);
    }
}

void CThreadPool::AddWorkers(uint8_t aCount)
{
    while (!!aCount--)
    {
        IThread::Ptr lWorker;
        if (!m_ParkingThreads.empty())
        {
            lWorker = m_ParkingThreads.back();
            m_ParkingThreads.pop_back();
            lWorker->SetDequeTasksNonLock(m_Tasks);
        }
        else
            lWorker = std::make_shared<CWorker>(m_Tasks);

        m_Threads.push_back(lWorker);
    }
}

IThreadPool::Ptr CreateThreadPool(unsigned int aCountStartThreads, IStrategyExpansion::Ptr aExpansion)
{
    return std::make_shared<CThreadPool>(aCountStartThreads, aExpansion);
}

}
}

#include <threadpoolex/core/IThreadPool.hpp>
#include <threadpoolex/core/TLockingForce.hpp>
#include <threadpoolex/core/TNotifier.hpp>
#include <threadpoolex/core/RAII.hpp>

#include <vector>
#include <deque>
#include <future>

namespace threadpoolex {
namespace core {
namespace {
//---------------------------------------------------------------

using TDequeTasks = TNotifier<TLockingEx<std::deque<ITask::Ptr>>>;

//---------------------------------------------------------------
class IThread
{
public:
    using Ptr = std::shared_ptr<IThread>;

public:
    virtual ~IThread() = default;
};

}
//---------------------------------------------------------------
class CWorker
    :public IThread
{
public:
    explicit CWorker(std::shared_ptr<TDequeTasks> aTasks);
    virtual ~CWorker();

protected:
    void Run();

private:
    std::shared_ptr<TDequeTasks> m_Tasks;
    thread_join_raii m_ThreadRaii;
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
    m_Tasks->notify_one();
}

void CWorker::Run()
{
    m_ThreadRaii = thread_join_raii(std::thread(
    [this](std::shared_ptr<TDequeTasks> aTasks)
    {
        while (true)
        {
            if (m_Stop)
                break;

            if (aTasks->empty())
                aTasks->wait();

            if (m_Stop)
                break;

            {
                ITask::Ptr lTask;
                {
                    lock_guard_ex<std::shared_ptr<TDequeTasks>> lock(aTasks);

                    if (!aTasks->empty())
                    {
                        lTask = aTasks->front();
                        aTasks->pop_front();
                    }
                }

                if (lTask)
                    lTask->Execute();
            }
        }
    }, m_Tasks));
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

private:
    std::vector<IThread::Ptr> m_Threads;
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
    force_lock_guard_ex<std::shared_ptr<TDequeTasks>> lock(m_Tasks);
    TryExpansionNonLock();
    m_Tasks->push_back(aTask);
    m_Tasks->notify_one();
}

void CThreadPool::AddTasks(const std::vector<ITask::Ptr>& aTasks)
{
    force_lock_guard_ex<std::shared_ptr<TDequeTasks>> lock(m_Tasks);
    TryExpansionNonLock();
    m_Tasks->insert(m_Tasks->end(), aTasks.begin(), aTasks.end());
    m_Tasks->notify_all();
}

void CThreadPool::AddTaskToTop(ITask::Ptr aTask)
{
    force_lock_guard_ex<std::shared_ptr<TDequeTasks>> lock(m_Tasks);
    TryExpansionNonLock();
    m_Tasks->push_front(aTask);
    m_Tasks->notify_one();
}

void CThreadPool::TryExpansion()
{
    force_lock_guard_ex<std::shared_ptr<TDequeTasks>> lock(m_Tasks);
    TryExpansionNonLock();
}

void CThreadPool::TryExpansionNonLock()
{
    if (m_Expansion->NeedExpansion(m_Tasks->size(), m_Threads.size()))
        m_Threads.push_back(std::make_shared<CWorker>(m_Tasks));
}

IThreadPool::Ptr CreateThreadPool(unsigned int aCountStartThreads, IStrategyExpansion::Ptr aExpansion)
{
    return std::make_shared<CThreadPool>(aCountStartThreads, aExpansion);
}

}
}
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

    virtual void Run() = 0;
    virtual bool IsWorking() = 0;
};

}
//---------------------------------------------------------------
class CThread
    :public IThread
{
public:
    explicit CThread(std::shared_ptr<TDequeTasks> aTasks);
    virtual ~CThread();

    virtual void Run() override;
    virtual bool IsWorking() override;

private:
    std::shared_ptr<TDequeTasks> m_Tasks;
    std::future<void> m_Thread;
    std::atomic_bool m_Stop;
    std::atomic_bool m_Working;
};

CThread::CThread(std::shared_ptr<TDequeTasks> aTasks)
    :m_Tasks(aTasks), m_Stop(false), m_Working(false)
{
    Run();
}

CThread::~CThread()
{
    m_Stop = true;
    m_Tasks->notify_one();
    if (m_Thread.valid())
        m_Thread.wait();
}

void CThread::Run()
{
    m_Thread = std::async(std::launch::async,
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
                set_values_raii<std::atomic_bool> lWorkingFlag(m_Working, true, false);
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
                    lTask->Execite();
            }
        }
    }, m_Tasks);
}

bool CThread::IsWorking()
{
    return m_Working;
}
//------------------------------------------------------------------

class CThreadPool
    :public IThreadPool
{
public:
    explicit CThreadPool(unsigned int aNumberThreads);

    virtual void AddTask(ITask::Ptr) override;
    virtual void AddTaskToTop(ITask::Ptr) override;

private:
    std::vector<IThread::Ptr> m_Threads;
    std::shared_ptr<TDequeTasks> m_Tasks;
};

CThreadPool::CThreadPool(unsigned int aNumberThreads)
    :m_Tasks(std::make_shared<TDequeTasks>())
{
    for (unsigned int i = 0; i < aNumberThreads; i++)
        m_Threads.push_back(std::make_shared<CThread>(m_Tasks));
}

void CThreadPool::AddTask(ITask::Ptr aTask)
{
    force_lock_guard_ex<std::shared_ptr<TDequeTasks>> lock(m_Tasks);
    m_Tasks->push_back(aTask);
    m_Tasks->notify_one();
}

void CThreadPool::AddTaskToTop(ITask::Ptr aTask)
{
    force_lock_guard_ex<std::shared_ptr<TDequeTasks>> lock(m_Tasks);
    m_Tasks->push_front(aTask);
    m_Tasks->notify_one();
}

IThreadPool::Ptr CreateThreadPool(unsigned int aNumberThreads)
{
    return std::make_shared<CThreadPool>(aNumberThreads);
}

}
}
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <threadpoolex/core/ITaskWait.hpp>
#include <future>

using namespace threadpoolex::core;
//--------------------------------------------------------------------------------------------------------------------------------------
class WaitingTask
    :public ITask, virtual CBaseObservableTask
{
public:
    WaitingTask(std::atomic_int& aReady, std::mutex& aMutex)
        :m_Ready(aReady), m_Mutex(aMutex)
    {}

    void Execute()
    {
        ++m_Ready; 
        m_Mutex.lock();
        m_Mutex.unlock();
    }

private:
    std::atomic_int& m_Ready;
    std::mutex& m_Mutex;
};

//--------------------------------------------------------------------------------------------------------------------------------------
class CTaskWait_test
    :public ::testing::Test
{
public:
    void SetUp()
    {}
};

//--------------------------------------------------------------------------------------------------------------------------------------
TEST_F(CTaskWait_test, simple)
{
    std::atomic_int lReady = 0;
    std::mutex lMutex;
    lMutex.lock();

    IWait::Ptr waiter;
    ITask::Ptr lTask = CreateTaskWait(std::make_shared<WaitingTask>(lReady, lMutex), waiter);
    {
        std::thread lTestThread([lTask]() {lTask->Execute(); });
        while (lReady != 1);

        lMutex.unlock();
        waiter->Wait();
        lTestThread.join();
    }
}

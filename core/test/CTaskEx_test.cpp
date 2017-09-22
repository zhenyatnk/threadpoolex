#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <threadpoolex/core/ITaskEx.hpp>
#include <future>

using namespace threadpoolex::core;
//--------------------------------------------------------------------------------------------------------------------------------------
class MockTask
    :public ITask
{
public:
    MOCK_METHOD0(Execute, void());
};

class WaitingTask
    :public ITask
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
class CTaskEx_test
    :public ::testing::Test
{
public:
    void SetUp()
    {}
};

//--------------------------------------------------------------------------------------------------------------------------------------
TEST_F(CTaskEx_test, non_execute_GetTimeStart)
{
    auto lTaskOrigin = std::make_shared<MockTask>();
    ITaskEx::Ptr lTask = CreateTaskEx(lTaskOrigin);
    ASSERT_EQ(std::chrono::steady_clock::time_point(), lTask->GetTimeStart());
}

TEST_F(CTaskEx_test, non_execute_GetTimeEnd)
{
    auto lTaskOrigin = std::make_shared<MockTask>();
    ITaskEx::Ptr lTask = CreateTaskEx(lTaskOrigin);
    ASSERT_EQ(std::chrono::steady_clock::time_point(), lTask->GetTimeEnd());
}

TEST_F(CTaskEx_test, non_execute_IsProcessing)
{
    auto lTaskOrigin = std::make_shared<MockTask>();
    ITaskEx::Ptr lTask = CreateTaskEx(lTaskOrigin);
    ASSERT_EQ(false, lTask->IsProcessing());
}

TEST_F(CTaskEx_test, execute_call)
{
    auto lTaskOrigin = std::make_shared<MockTask>();
    ITaskEx::Ptr lTask = CreateTaskEx(lTaskOrigin);

    EXPECT_CALL(*lTaskOrigin, Execute()).Times(1);

    lTask->Execute();
}

TEST_F(CTaskEx_test, execute_IsProcessing)
{
    std::atomic_int lReady = 0;
    std::mutex lMutex;
    lMutex.lock();

    ITaskEx::Ptr lTask = CreateTaskEx(std::make_shared<WaitingTask>(lReady, lMutex));
    {
        std::thread lTestThread([lTask]() {lTask->Execute(); });
        while (lReady != 1);

        ASSERT_EQ(true, lTask->IsProcessing());
        lMutex.unlock();
        lTestThread.join();
    }
    ASSERT_EQ(false, lTask->IsProcessing());
}

TEST_F(CTaskEx_test, execute_GetTimeStart)
{
    std::atomic_int lReady = 0;
    std::mutex lMutex;
    lMutex.lock();

    ITaskEx::Ptr lTask = CreateTaskEx(std::make_shared<WaitingTask>(lReady, lMutex));
    std::thread lTestThread([lTask]() {lTask->Execute(); });
    while (lReady != 1);

    ASSERT_NE(std::chrono::steady_clock::time_point(), lTask->GetTimeStart());
    lMutex.unlock();
    lTestThread.join();
}

TEST_F(CTaskEx_test, execute_GetTimeEnd)
{
    std::atomic_int lReady = 0;
    std::mutex lMutex;
    lMutex.lock();

    ITaskEx::Ptr lTask = CreateTaskEx(std::make_shared<WaitingTask>(lReady, lMutex));
    std::thread lTestThread([lTask]() {lTask->Execute(); });
    while (lReady != 1);

    lMutex.unlock();
    lTestThread.join();

    ASSERT_NE(std::chrono::steady_clock::time_point(), lTask->GetTimeEnd());
}

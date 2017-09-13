#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <threadpoolex/core/ITimerActive.hpp>

#include <atomic>
#include <memory>

using namespace threadpoolex::core;
using namespace ::testing;
//--------------------------------------------------------------------------------------------------------------------------------------
class IObserverTimer_mock
    :public IObserverTimer
{
public:
    MOCK_METHOD0(HandleCheck, void());
    MOCK_METHOD0(HandleClose, void());
    MOCK_METHOD2(HandleError, void(const std::string &, const int&));
};

//--------------------------------------------------------------------------------------------------------------------------------------
class IObserverTimer_ready
    :public IObserverTimer
{
public:
    explicit IObserverTimer_ready(std::atomic_int &aCheckReady)
        :m_CheckReady(aCheckReady)
    {}

    virtual void HandleCheck() override
    {
        ++m_CheckReady;
    }

    virtual void HandleClose() override
    {}

    virtual void HandleError(const std::string &, const int& ) override
    {}

private:
    std::atomic_int &m_CheckReady;
};
//--------------------------------------------------------------------------------------------------------------------------------------
class IObserverTimer_ready_check_exception
    :public IObserverTimer
{
public:
    explicit IObserverTimer_ready_check_exception(std::atomic_int &aCheckReady)
        :m_CheckReady(aCheckReady)
    {}

    virtual void HandleCheck() override
    {
        ++m_CheckReady;
        throw std::runtime_error("test error");
    }

    virtual void HandleClose() override
    {}
    
    virtual void HandleError(const std::string &, const int&) override
    {}

private:
    std::atomic_int &m_CheckReady;
};

class IObserverTimer_ready_close_exception
    :public IObserverTimer
{
public:
    explicit IObserverTimer_ready_close_exception(std::atomic_int &aCloseReady)
        :m_CloseReady(aCloseReady)
    {}

    virtual void HandleCheck() override
    {}

    virtual void HandleClose() override
    {
        ++m_CloseReady;
        throw std::runtime_error("test error");
    }

    virtual void HandleError(const std::string &, const int&) override
    {}

private:
    std::atomic_int &m_CloseReady;
};

//--------------------------------------------------------------------------------------------------------------------------------------
class CTimerActive_test
    :public ::testing::Test
{
public:
    void SetUp()
    {}
};

//--------------------------------------------------------------------------------------------------------------------------------------
TEST_F(CTimerActive_test, infinity_OnClose)
{
    auto lObserver = std::make_shared<IObserverTimer_mock>();
    EXPECT_CALL(*lObserver, HandleClose()).Times(1);
    {
        ITimerActive::Ptr lTimer = CreateTimerActive(500);
        lTimer->AddObserver(lObserver);
    }
}

TEST_F(CTimerActive_test, infinity_OnCheck)
{
    std::atomic_int lCheckReady = 0;
    auto lObserver = std::make_shared<IObserverTimer_ready>(lCheckReady);

    ITimerActive::Ptr lTimer = CreateTimerActive(1);
    lTimer->AddObserver(lObserver);
    while (lCheckReady < 5);
}

TEST_F(CTimerActive_test, infinity_exception_OnClose)
{
    std::atomic_int lCloseReady = 0;
    auto lObserver = std::make_shared<IObserverTimer_ready_close_exception>(lCloseReady);
    {
        ITimerActive::Ptr lTimer = CreateTimerActive(1);
        lTimer->AddObserver(lObserver);
    }
    ASSERT_EQ(1, lCloseReady);
}

TEST_F(CTimerActive_test, infinity_exception_OnCheck)
{
    std::atomic_int lCheckReady = 0;
    auto lObserver = std::make_shared<IObserverTimer_ready_check_exception>(lCheckReady);

    ITimerActive::Ptr lTimer = CreateTimerActive(1);
    lTimer->AddObserver(lObserver);
    while (lCheckReady < 5);
}

TEST_F(CTimerActive_test, infinity_exception_second_true_OnCheck)
{
    std::atomic_int lCheckReady = 0;
    auto lObserver = std::make_shared<IObserverTimer_ready_check_exception>(lCheckReady);
    auto lObserverMock = std::make_shared<IObserverTimer_mock>();
    EXPECT_CALL(*lObserverMock, HandleError(_, _)).Times(AtMost(5));
    EXPECT_CALL(*lObserverMock, HandleCheck()).Times(AtMost(5));
    EXPECT_CALL(*lObserverMock, HandleClose()).Times(1);

    ITimerActive::Ptr lTimer = CreateTimerActive(1);
    lTimer->AddObserver(lObserver);
    lTimer->AddObserver(lObserverMock);
    while (lCheckReady < 1);
}

TEST_F(CTimerActive_test, noninfinity_zero_OnClose)
{
    auto lObserver = std::make_shared<IObserverTimer_mock>();
    EXPECT_CALL(*lObserver, HandleClose()).Times(1);
    {
        ITimerActive::Ptr lTimer = CreateTimerActive(500, 0);
        lTimer->AddObserver(lObserver);
    }
}

TEST_F(CTimerActive_test, noninfinity_nonzero_OnClose)
{
    auto lObserver = std::make_shared<IObserverTimer_mock>();
    EXPECT_CALL(*lObserver, HandleClose()).Times(1);
    {
        ITimerActive::Ptr lTimer = CreateTimerActive(500, 2);
        lTimer->AddObserver(lObserver);
    }
}

TEST_F(CTimerActive_test, noninfinity_OnCheck)
{
    std::atomic_int lCheckReady = 0;
    auto lObserver = std::make_shared<IObserverTimer_ready>(lCheckReady);

    ITimerActive::Ptr lTimer = CreateTimerActive(1, 4);
    lTimer->AddObserver(lObserver);
    while (lCheckReady < 4);
}

TEST_F(CTimerActive_test, noninfinity_exception_OnClose)
{
    std::atomic_int lCloseReady = 0;
    auto lObserver = std::make_shared<IObserverTimer_ready_close_exception>(lCloseReady);
    {
        ITimerActive::Ptr lTimer = CreateTimerActive(1, 4);
        lTimer->AddObserver(lObserver);
    }
    ASSERT_EQ(1, lCloseReady);
}

TEST_F(CTimerActive_test, noninfinity_exception_OnCheck)
{
    std::atomic_int lCheckReady = 0;
    auto lObserver = std::make_shared<IObserverTimer_ready_check_exception>(lCheckReady);

    ITimerActive::Ptr lTimer = CreateTimerActive(1, 4);
    lTimer->AddObserver(lObserver);
    while (lCheckReady < 4);
}

TEST_F(CTimerActive_test, noninfinity_exception_second_true_OnCheck)
{
    std::atomic_int lCheckReady = 0;
    auto lObserver = std::make_shared<IObserverTimer_ready_check_exception>(lCheckReady);
    auto lObserverMock = std::make_shared<IObserverTimer_mock>();
    EXPECT_CALL(*lObserverMock, HandleError(_, _)).Times(4);
    EXPECT_CALL(*lObserverMock, HandleCheck()).Times(4);
    EXPECT_CALL(*lObserverMock, HandleClose()).Times(1);

    ITimerActive::Ptr lTimer = CreateTimerActive(1, 4);
    lTimer->AddObserver(lObserver);
    lTimer->AddObserver(lObserverMock);
    while (lCheckReady < 4);
}
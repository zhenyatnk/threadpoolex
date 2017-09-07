#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <threadpoolex/core/ITimer.hpp>

#include <atomic>
#include <memory>

using namespace threadpoolex::core;
//--------------------------------------------------------------------------------------------------------------------------------------
class IHandlerTimer_mock
    :public IHandlerTimer
{
public:
    MOCK_METHOD0(OnCheck, void());
    MOCK_METHOD0(OnClose, void());
};

//--------------------------------------------------------------------------------------------------------------------------------------
class IHandlerTimer_ready
    :public IHandlerTimer
{
public:
    explicit IHandlerTimer_ready(std::atomic_int &aCheckReady)
        :m_CheckReady(aCheckReady)
    {}

    virtual void OnCheck() override
    {
        ++m_CheckReady;
    }

    virtual void OnClose() override
    {}

private:
    std::atomic_int &m_CheckReady;
};
//--------------------------------------------------------------------------------------------------------------------------------------
class IHandlerTimer_ready_check_exception
    :public IHandlerTimer
{
public:
    explicit IHandlerTimer_ready_check_exception(std::atomic_int &aCheckReady)
        :m_CheckReady(aCheckReady)
    {}

    virtual void OnCheck() override
    {
        ++m_CheckReady;
        throw std::runtime_error("test error");
    }

    virtual void OnClose() override
    {}

private:
    std::atomic_int &m_CheckReady;
};

class IHandlerTimer_ready_close_exception
    :public IHandlerTimer
{
public:
    explicit IHandlerTimer_ready_close_exception(std::atomic_int &aCloseReady)
        :m_CloseReady(aCloseReady)
    {}

    virtual void OnCheck() override
    {
    }

    virtual void OnClose() override
    {
        ++m_CloseReady;
        throw std::runtime_error("test error");
    }

private:
    std::atomic_int &m_CloseReady;
};

//--------------------------------------------------------------------------------------------------------------------------------------
class CTimer_test
    :public ::testing::Test
{
public:
    void SetUp()
    {}
};

//--------------------------------------------------------------------------------------------------------------------------------------
TEST_F(CTimer_test, infinity_OnClose)
{
    auto lHandler = std::make_shared<IHandlerTimer_mock>();
    EXPECT_CALL(*lHandler, OnClose()).Times(1);
    {
        ITimer::Ptr lTimer = CreateTimer(500);
        lTimer->AddHandler(lHandler);
    }
}

TEST_F(CTimer_test, infinity_OnCheck)
{
    std::atomic_int lCheckReady = 0;
    auto lHandler = std::make_shared<IHandlerTimer_ready>(lCheckReady);

    ITimer::Ptr lTimer = CreateTimer(1);
    lTimer->AddHandler(lHandler);
    while (lCheckReady < 5);
}

TEST_F(CTimer_test, infinity_exception_OnClose)
{
    std::atomic_int lCloseReady = 0;
    auto lHandler = std::make_shared<IHandlerTimer_ready_close_exception>(lCloseReady);
    {
        ITimer::Ptr lTimer = CreateTimer(1);
        lTimer->AddHandler(lHandler);
    }
    ASSERT_EQ(1, lCloseReady);
}

TEST_F(CTimer_test, infinity_exception_OnCheck)
{
    std::atomic_int lCheckReady = 0;
    auto lHandler = std::make_shared<IHandlerTimer_ready_check_exception>(lCheckReady);

    ITimer::Ptr lTimer = CreateTimer(1);
    lTimer->AddHandler(lHandler);
    while (lCheckReady < 5);
}

TEST_F(CTimer_test, noninfinity_zero_OnClose)
{
    auto lHandler = std::make_shared<IHandlerTimer_mock>();
    EXPECT_CALL(*lHandler, OnClose()).Times(1);
    {
        ITimer::Ptr lTimer = CreateTimer(500, 0);
        lTimer->AddHandler(lHandler);
    }
}

TEST_F(CTimer_test, noninfinity_nonzero_OnClose)
{
    auto lHandler = std::make_shared<IHandlerTimer_mock>();
    EXPECT_CALL(*lHandler, OnClose()).Times(1);
    {
        ITimer::Ptr lTimer = CreateTimer(500, 2);
        lTimer->AddHandler(lHandler);
    }
}

TEST_F(CTimer_test, noninfinity_OnCheck)
{
    std::atomic_int lCheckReady = 0;
    auto lHandler = std::make_shared<IHandlerTimer_ready>(lCheckReady);

    ITimer::Ptr lTimer = CreateTimer(1, 4);
    lTimer->AddHandler(lHandler);
    while (lCheckReady < 4);
}

TEST_F(CTimer_test, noninfinity_exception_OnClose)
{
    std::atomic_int lCloseReady = 0;
    auto lHandler = std::make_shared<IHandlerTimer_ready_close_exception>(lCloseReady);
    {
        ITimer::Ptr lTimer = CreateTimer(1, 4);
        lTimer->AddHandler(lHandler);
    }
    ASSERT_EQ(1, lCloseReady);
}

TEST_F(CTimer_test, noninfinity_exception_OnCheck)
{
    std::atomic_int lCheckReady = 0;
    auto lHandler = std::make_shared<IHandlerTimer_ready_check_exception>(lCheckReady);

    ITimer::Ptr lTimer = CreateTimer(1, 4);
    lTimer->AddHandler(lHandler);
    while (lCheckReady < 4);
}
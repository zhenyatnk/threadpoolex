#include <gtest/gtest.h>
#include <threadpoolex/core/TLockingShared.hpp>
#include <future>

//--------------------------------------------------------------------------------------------------------------------------------------
using namespace threadpoolex::core;
namespace{

class TestObject
{
public:
    TestObject()
        :a(0)
    {}
    int a;
};

class TestObjectValueRef
{
public:
    explicit TestObjectValueRef(int& x)
        :a(x)
    {}
    int &a;
};

class TestObjectConstValue
{
public:
    explicit TestObjectConstValue(int x)
        :a(x)
    {}
    int a;
};
}

class TLockingShared_test
    :public ::testing::Test
{
public:
    void SetUp()
    {
    }

};
//--------------------------------------------------------------------------------------------------------------------------------------
TEST_F(TLockingShared_test, void_lock_unlock)
{
    TLockingShared<void> aTest;
    aTest.lock();
    aTest.unlock();
}

TEST_F(TLockingShared_test, parameters_lvalue_ctor)
{
    TestObjectConstValue v(10);
    TLockingShared<TestObjectConstValue> aTest(v);
    v.a = 25;
    ASSERT_EQ(10, aTest.a);
}

TEST_F(TLockingShared_test, parameters_rvalue_ctor)
{
    TLockingShared<TestObjectConstValue> aTest(TestObjectConstValue(5));
    ASSERT_EQ(5, aTest.a);
}

TEST_F(TLockingShared_test, void_lock_other_thread)
{
    TLockingShared<void> aTest;
    std::vector<std::string> lValue;
    {
        aTest.lock();
        std::atomic_int lReadyThreads = 0;

        auto lThread = std::thread([&]()
        {
            ++lReadyThreads;
            aTest.lock();
            lValue.push_back("in thread");
            aTest.unlock();
        });

        while (lReadyThreads != 1);

        lValue.push_back("in function");

        ASSERT_EQ(1, lValue.size());
        ASSERT_STREQ("in function", lValue[0].c_str());

        aTest.unlock();
        lThread.join();
    }
    ASSERT_EQ(2, lValue.size());
    ASSERT_STREQ("in thread", lValue[1].c_str());
}

TEST_F(TLockingShared_test, object_lock_unlock)
{
    TLockingShared<TestObject> aTest;
    aTest.lock();
    aTest.unlock();
}

TEST_F(TLockingShared_test, shared_shared)
{
    TLockingShared<void> aTest;
    aTest.shared_lock();
    aTest.shared_lock();
    aTest.shared_lock();
    aTest.shared_unlock();
    aTest.shared_unlock();
    aTest.shared_unlock();
}

TEST_F(TLockingShared_test, shared_nonshared)
{
    TLockingShared<void> aTest;
    aTest.shared_lock();
    aTest.shared_lock();
    aTest.shared_unlock();
    aTest.shared_unlock();
    
    aTest.lock();
    aTest.unlock();
}

TEST_F(TLockingShared_test, nonshared_shared)
{
    TLockingShared<void> aTest;
    aTest.lock();
    aTest.unlock();

    aTest.shared_lock();
    aTest.shared_lock();
    aTest.shared_unlock();
    aTest.shared_unlock();
}

TEST_F(TLockingShared_test, nonshared_shared_thread)
{
    TLockingShared<void> aTest;
    aTest.lock();
    std::atomic_bool lReady = false;

    auto thread = std::thread([&]()
    {
        lReady = true;
        aTest.shared_lock();
        aTest.shared_lock();
        aTest.shared_unlock();
        aTest.shared_unlock();
    });

    while (!lReady);

    aTest.unlock();
    thread.join();
}

TEST_F(TLockingShared_test, nonshared_shared_many_thread)
{
    unsigned countThreads = 20;
    TLockingShared<void> aTest;
    std::atomic_int lReadyThreads = 0;

    std::condition_variable lCondition;
    std::mutex lMutex;
    std::atomic_bool lReally = false;

    auto lfunc = [&]()
    {
        aTest.shared_lock();
        {
            std::unique_lock<std::mutex> lk(lMutex);
            ++lReadyThreads;
            lCondition.wait(lk, [&]() {return lReally.load(); });
        }
        aTest.shared_unlock();
    };

    std::vector<std::thread> lThreads;

    for (unsigned lIndex = 0; lIndex < countThreads; ++lIndex)
        lThreads.push_back(std::thread(lfunc));

    while (lReadyThreads != countThreads);

    {
        std::unique_lock<std::mutex> lk(lMutex);
        lReally = true;
        lCondition.notify_all();
    }

    for (auto & thread : lThreads)
        thread.join();
}

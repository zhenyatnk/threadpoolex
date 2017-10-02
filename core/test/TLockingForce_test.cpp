#include <gtest/gtest.h>
#include <threadpoolex/core/TLockingForce.hpp>
#include <future>

//--------------------------------------------------------------------------------------------------------------------------------------
using namespace threadpoolex::core;
namespace{

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

class TLockingEx_test
    :public ::testing::Test
{
public:
    void SetUp()
    {
    }

};
//--------------------------------------------------------------------------------------------------------------------------------------
TEST_F(TLockingEx_test, void_lock_unlock)
{
    TLockingEx<void> aTest;
    aTest.lock();
    aTest.unlock();
}

TEST_F(TLockingEx_test, parameters_lvalue_ctor)
{
    TestObjectConstValue v(10);
    TLockingEx<TestObjectConstValue> aTest(v);
    v.a = 25;
    ASSERT_EQ(10, aTest.a);
}

TEST_F(TLockingEx_test, parameters_rvalue_ctor)
{
    TLockingEx<TestObjectConstValue> aTest(TestObjectConstValue(5));
    ASSERT_EQ(5, aTest.a);
}

TEST_F(TLockingEx_test, void_lock_other_thread)
{
    TLockingEx<void> aTest;
    std::vector<std::string> aValue;
    {
        aTest.lock();
        std::atomic_int lReadyThreads = 0;

        auto lThread = std::thread([&]()
        {
            ++lReadyThreads;
            aTest.lock();
            aValue.push_back("in thread");
            aTest.unlock();
        });

        while (lReadyThreads != 1);

        aValue.push_back("in function");

        ASSERT_EQ(1, aValue.size());
        ASSERT_STREQ("in function", aValue[0].c_str());

        aTest.unlock();
        lThread.join();
    }
    ASSERT_EQ(2, aValue.size());
    ASSERT_STREQ("in thread", aValue[1].c_str());
}

TEST_F(TLockingEx_test, object_lock_unlock)
{
    TLockingEx<std::vector<int>> aTest;
    aTest.lock();
    aTest.unlock();
}

TEST_F(TLockingEx_test, object_lock_other_thread)
{
    TLockingEx<std::vector<std::string>> aTest;
    {
        aTest.lock();
        std::atomic_int lReadyThreads = 0;

        auto lThread = std::thread([&]()
        {
            ++lReadyThreads;
            aTest.lock();
            aTest.push_back("in thread");
            aTest.unlock();
        });
        
        while (lReadyThreads != 1);

        aTest.push_back("in function");

        ASSERT_EQ(1, aTest.size());
        ASSERT_STREQ("in function", aTest[0].c_str());

        aTest.unlock();
        lThread.join();
    }
    ASSERT_EQ(2, aTest.size());
    ASSERT_STREQ("in thread", aTest[1].c_str());
}

TEST_F(TLockingEx_test, object_exclock_lock_other_thread)
{
    unsigned numbersSimpleLock = 25;
    TLockingEx<std::vector<std::string>> aTest;
    {
        std::atomic_int lReadyThreads = 0;
        aTest.lock();

        auto lfunc_lock_simple = [&]()
        {
            ++lReadyThreads;
            aTest.lock();
            aTest.push_back("lock");
            aTest.unlock();
        };

        auto lfunc_exlock_simple = [&]()
        {
            ++lReadyThreads;
            aTest.force_lock();
            aTest.push_back("exclusive_lock");
            aTest.force_unlock();
        };

        std::vector<std::thread> lThreads;
        
        for (unsigned lIndex = 0; lIndex < numbersSimpleLock; ++lIndex) 
            lThreads.push_back(std::thread(lfunc_lock_simple));
        lThreads.push_back(std::thread(lfunc_exlock_simple));

        while(lReadyThreads != numbersSimpleLock + 1);

        aTest.unlock();

        for (auto & thread : lThreads)
            thread.join();
    }
    ASSERT_EQ(numbersSimpleLock + 1, aTest.size());
    ASSERT_STREQ("exclusive_lock", aTest[0].c_str());
}
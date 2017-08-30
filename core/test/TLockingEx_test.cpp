#include <gtest/gtest.h>
#include <threadpoolex/core/TLockingEx.hpp>
#include <future>

//--------------------------------------------------------------------------------------------------------------------------------------
using namespace threadpoolex::core;

class TLockingEx_test
    :public ::testing::Test
{
public:
    void SetUp()
    {
    }

};
//--------------------------------------------------------------------------------------------------------------------------------------
TEST_F(TLockingEx_test, lock_unlock)
{
    TLockingEx<std::vector<int>> aTest;
    aTest.lock();
    aTest.unlock();
}

TEST_F(TLockingEx_test, lock_other_thread)
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

TEST_F(TLockingEx_test, exclock_lock_other_thread)
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
            aTest.push_back("simple_lock");
            aTest.unlock();
        };

        auto lfunc_exlock_simple = [&]()
        {
            ++lReadyThreads;
            aTest.exclusive_lock();
            aTest.push_back("exclusive_lock");
            aTest.exclusive_unlock();
        };

        std::vector<std::thread> lThreads;
        
        for (auto lIndex = 0; lIndex < numbersSimpleLock; ++lIndex) 
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
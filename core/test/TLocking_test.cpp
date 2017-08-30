#include <gtest/gtest.h>
#include <threadpoolex/core/TLocking.hpp>
#include <future>

//--------------------------------------------------------------------------------------------------------------------------------------
using namespace threadpoolex::core;
class TestObject
{
public:
    TestObject()
        :a(0)
    {}
    int a;
};

class TLocking_test
    :public ::testing::Test
{
public:
    void SetUp()
    {
    }

};
//--------------------------------------------------------------------------------------------------------------------------------------
TEST_F(TLocking_test, lock_unlock)
{
    TLocking<TestObject> aTest;
    aTest.lock();
    aTest.unlock();
}

TEST_F(TLocking_test, lock_other_thread)
{
    TLocking<std::vector<std::string>> aTest;
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
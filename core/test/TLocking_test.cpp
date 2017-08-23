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
    TLocking<TestObject> aTest;
    {
        aTest.lock();

        auto lfuture = std::async(std::launch::async, [&aTest]()
        {
            aTest.lock();
            aTest.a += 5;
            aTest.unlock();
        });
        aTest.a += 10;
        ASSERT_EQ(10, aTest.a);

        aTest.unlock();
    }
    ASSERT_EQ(15, aTest.a);

}
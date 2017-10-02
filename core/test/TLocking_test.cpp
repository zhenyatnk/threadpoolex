#include <gtest/gtest.h>
#include <threadpoolex/core/TLocking.hpp>
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

class TLocking_test
    :public ::testing::Test
{
public:
    void SetUp()
    {
    }

};
//--------------------------------------------------------------------------------------------------------------------------------------
TEST_F(TLocking_test, void_lock_unlock)
{
    TLocking<void> aTest;
    aTest.lock();
    aTest.unlock();
}
TEST_F(TLocking_test, parameters_lvalue_ctor)
{
    TestObjectConstValue v(10);
    TLocking<TestObjectConstValue> aTest(v);
    v.a = 25;
    ASSERT_EQ(10, aTest.a);
}

TEST_F(TLocking_test, parameters_rvalue_ctor)
{
    TLocking<TestObjectConstValue> aTest(TestObjectConstValue(5));
    ASSERT_EQ(5, aTest.a);
}

TEST_F(TLocking_test, void_lock_other_thread)
{
    TLocking<void> aTest;
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

TEST_F(TLocking_test, object_lock_unlock)
{
    TLocking<TestObject> aTest;
    aTest.lock();
    aTest.unlock();
}

TEST_F(TLocking_test, object_lock_other_thread)
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
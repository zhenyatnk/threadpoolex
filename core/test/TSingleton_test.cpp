#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <threadpoolex/core/TSingleton.hpp>
#include <future>

using namespace threadpoolex::core;

SINGLETON(int, []() {return 5; }())

//--------------------------------------------------------------------------------------------------------------------------------------
class TSingleton_test
    :public ::testing::Test
{
public:
    void SetUp()
    {}
};
//--------------------------------------------------------------------------------------------------------------------------------------
TEST_F(TSingleton_test, empty)
{
    Singleton_int::GetInstance();
}

TEST_F(TSingleton_test, get_value)
{
    ASSERT_EQ(5, Singleton_int::GetInstance()());
}

TEST_F(TSingleton_test, set_value)
{
    Singleton_int::GetInstance()() = 10;
    ASSERT_EQ(10, Singleton_int::GetInstance()());
}

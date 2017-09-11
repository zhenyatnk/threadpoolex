#include <gtest/gtest.h>
#include <threadpoolex/core/TNotifier.hpp>
#include <future>

//--------------------------------------------------------------------------------------------------------------------------------------
using namespace threadpoolex::core;

class TNotifier_test
    :public ::testing::Test
{
public:
    void SetUp()
    {}

};
//--------------------------------------------------------------------------------------------------------------------------------------
TEST_F(TNotifier_test, void_empty)
{
    TNotifier<void> aTest;
}

TEST_F(TNotifier_test, void_notify_one)
{
    unsigned countThreads = 2;
    std::atomic_int lReadyThreads = 0;

    TNotifier<void> aTest;

    auto lfunc = [&]()
    {
        ++lReadyThreads;
        aTest.wait();
        --lReadyThreads;
    };
    
    std::vector<std::thread> lThreads;

    for (unsigned lIndex = 0; lIndex < countThreads; ++lIndex)
        lThreads.push_back(std::thread(lfunc));
    
    while (lReadyThreads != countThreads);
    
    aTest.notify_one();
    
    while (lReadyThreads != countThreads - 1);

    aTest.notify_one();

    for (auto & thread : lThreads)
        thread.join();
}

TEST_F(TNotifier_test, void_notify_all)
{
    unsigned countThreads = 20;
    std::atomic_int lReadyThreads = 0;

    TNotifier<void> aTest;

    auto lfunc = [&]()
    {
       aTest.wait([&]() { ++lReadyThreads; });
    };

    std::vector<std::thread> lThreads;

    for (unsigned lIndex = 0; lIndex < countThreads; ++lIndex)
        lThreads.push_back(std::thread(lfunc));

    while (lReadyThreads != countThreads);

    aTest.notify_all();

    for (auto & thread : lThreads)
        thread.join();
}

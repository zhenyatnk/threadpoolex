#include <gtest/gtest.h>
#include <threadpoolex/core/IStrategyExpansion.hpp>
#include <future>

//--------------------------------------------------------------------------------------------------------------------------------------
using namespace threadpoolex::core;

class CExpansionToMax_test
    :public ::testing::Test
{
public:
    void SetUp()
    {}
};

TEST_F(CExpansionToMax_test, max_empty)
{
    IStrategyExpansion::Ptr aTest = CreateExpansionToMax(0);
    ASSERT_EQ(0, aTest->GetOptimalDiffWorkers(10, 0));
}

TEST_F(CExpansionToMax_test, exist_less_max)
{
    IStrategyExpansion::Ptr aTest = CreateExpansionToMax(5);
    ASSERT_EQ(1, aTest->GetOptimalDiffWorkers(10, 4));
}

TEST_F(CExpansionToMax_test, exist_eq_max)
{
    IStrategyExpansion::Ptr aTest = CreateExpansionToMax(5);
    ASSERT_EQ(0, aTest->GetOptimalDiffWorkers(10, 5));
}

TEST_F(CExpansionToMax_test, exist_great_max)
{
    IStrategyExpansion::Ptr aTest = CreateExpansionToMax(5);
    ASSERT_EQ(0, aTest->GetOptimalDiffWorkers(10, 6));
}

TEST_F(CExpansionToMax_test, all_working_exist_less_max)
{
    IStrategyExpansion::Ptr aTest = CreateExpansionToMax(5);
    ASSERT_EQ(0, aTest->GetOptimalDiffWorkers(0, 4));
}

TEST_F(CExpansionToMax_test, all_working_exist_eq_max)
{
    IStrategyExpansion::Ptr aTest = CreateExpansionToMax(5);
    ASSERT_EQ(0, aTest->GetOptimalDiffWorkers(0, 5));
}

TEST_F(CExpansionToMax_test, all_working_exist_great_max)
{
    IStrategyExpansion::Ptr aTest = CreateExpansionToMax(5);
    ASSERT_EQ(0, aTest->GetOptimalDiffWorkers(0, 6));
}
extern "C" {
#include "functions.h"
}
#include <gtest/gtest.h>

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) 
{
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.
    EXPECT_EQ(7 * 6, 42);
}

TEST(sumValues_PASS, BasicAssertions) 
{
    int retVal = sumValues(12,11);
    EXPECT_EQ(retVal, 23);

}

TEST(sumValues_FAIL, BasicAssertions) 
{
    int retVal = sumValues(12,12);
    EXPECT_EQ(retVal, 23);

}

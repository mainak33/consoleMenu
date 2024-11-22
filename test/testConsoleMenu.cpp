#include "gtest/gtest.h"
#include "osUtils.h"
#include "ioUtils.h"
#include "svUtils.h"

using osUtils::OS;
using osUtils::clearScreen;
using ioUtils::getNumberInRange;
using ioUtils::IntegerString;
using DigitInString = IntegerString::DigitInString;

TEST(TestosUtils, TestOS) {
    #if defined(_WIN64)
        EXPECT_TRUE(OS::is(OS::NAME::WINDOWS));
        EXPECT_TRUE(OS::is(OS::NAME::WINDOWS64));
        EXPECT_FALSE(OS::is(OS::NAME::LINUX));
    #endif
    #if defined(_WIN32) && !defined(_WIN64)
        EXPECT_TRUE(OS::is(OS::NAME::WINDOWS));
        EXPECT_TRUE(OS::is(OS::NAME::WINDOWS32));
        EXPECT_FALSE(OS::is(OS::NAME::LINUX));
    #endif
    #if defined(__linux__)
        EXPECT_FALSE(OS::is(OS::NAME::WINDOWS));
        EXPECT_FALSE(OS::is(OS::NAME::WINDOWS32));
        EXPECT_TRUE(OS::is(OS::NAME::LINUX));
    #endif
}   

TEST(TestioUtils, TestIntegerString) {
    EXPECT_TRUE(IntegerString {"-0000007699806578356817" } < IntegerString{ "+000007" });
    EXPECT_TRUE(IntegerString{ "0" } == IntegerString{ "0000000000000" });
}
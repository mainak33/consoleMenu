#include "gtest/gtest.h"
#include "osUtils.h"
#include "ioUtils.h"
#include "svUtils.h"
#include <string>
#include <sstream>

using osUtils::OS;
using osUtils::clearScreen;
using ioUtils::getNumberInRange;
using ioUtils::IntegerString;
using DigitInString = IntegerString::DigitInString;
using std::string;
using std::stringstream;
using std::istringstream;
using std::ostringstream;
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
TEST(TestsvUtils, TestwrapToLength) {
    
    stringstream ostrstream{};

    string expectedString{
        "string_view lines th\n"
        "   ere is a a very l\n"
        "   ong line \n"
        "   .Something elseca\n"
        "   nbedone thisisave\n"
        "   rylongcontiguousw\n"
        "   ordwithoutadelimi\n"
        "   ter then thereare\n"
        "   three newlines\n"
        "   \n"
        "   \n"
        "   followedbysomelon\n"
        "   gibberish"
    };

    svUtils::wrapToLength(
        ostrstream,
        "string_view lines there is a a very long line \n.Something elsecanbedone thisisaverylongcontiguouswordwithoutadelimiter then thereare three newlines\n \n\nfollowedbysomelongibberish ",
        {
            "   ",
            20,
            " ",
            3
        }
    );
    EXPECT_EQ(ostrstream.str(), expectedString);
}

TEST(TestuserInput, TestgetNumberInRange) {
    istringstream is{2.5};
    ostringstream os;

    int x = getNumberInRange<int>(1, 6, "Choose an option from 1 to 5", is, os);
    //os >> 
    //cout << "Valid number : " << x << '\n';
    //IntegerString xi = getNumberInRange<IntegerString>({"1"}, {"3"}, "Choose an option from 1 or 2");
    //cout << "Valid option : " << xi << '\n';
}
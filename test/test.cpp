#include <c4/log/log.hpp>
#include <string>
#include <gtest/gtest.h>

namespace c4 {
namespace log {

std::string logrcv;
void test_logpump(const char* c_str, size_t len)
{
    logrcv.append(c_str, len);
}

struct SetupLogTest
{
    SetupLogTest()
    {
        logrcv.clear();
        c4::log::set_logpump(&test_logpump);
    }
    ~SetupLogTest()
    {
        c4::log::set_logpump(nullptr);
    }
};

template <class... Args>
void test_print(const char* expected, Args && ...args)
{
    {
        auto h = SetupLogTest();
        c4::log::print(std::forward<Args>(args)...);
    }
    EXPECT_STREQ(logrcv.c_str(), expected);
}

template <class... Args>
void test_printsep(const char* expected, Sep s, Args && ...args)
{
    {
        auto h = SetupLogTest();
        c4::log::printsep(s, std::forward<Args>(args)...);
    }
    EXPECT_STREQ(logrcv.c_str(), expected);
}

template <class... Args>
void test_log(const char* expected, csubstr fmt, Args && ...args)
{
    {
        auto h = SetupLogTest();
        c4::log::log(fmt, std::forward<Args>(args)...);
    }
    EXPECT_STREQ(logrcv.c_str(), expected);
}

TEST(log, print)
{
    test_print("FDX\n", "FDX");
    test_print("FDX 1 2 3\n", "FDX", 1, 2, 3);
}

TEST(log, printsep)
{
    test_printsep("FDX\n", sep('-'), "FDX");
    test_printsep("FDX-1-2-3\n", sep('-'), "FDX", 1, 2, 3);
}

TEST(log, log)
{
    test_log("the cat ate the mouse\n", "the {} ate the {}", "cat", "mouse");
}

} // namespace log
} // namespace c4

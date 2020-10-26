#include <c4/std/string.hpp>
#include <c4/log/log.hpp>
#include <string>
#include <gtest/gtest.h>

//------------------------------------------------------------------------

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
        logrcv.shrink_to_fit();
        c4::logns::set_logpump(&test_logpump);
    }
    ~SetupLogTest()
    {
        c4::logns::set_logpump(nullptr);
    }
};


//------------------------------------------------------------------------
// implementation detail: test there is a way to prevent
// the dump buffer from growing too big when large types are logged

void shrink_buf()
{
    auto * buf = c4::logns::detail::_dump_buf();
    buf->len = 8;
    buf->shrink_to_fit();
}

struct LargeType1
{
    std::vector<std::string> s;
};

struct LargeType2
{
    std::vector<std::string> s;
};

// if we implement logging through to_chars(), the dump buffer will have the sum of all member sizes
size_t to_chars(c4::substr buf, LargeType1 const& v)
{
    size_t total = 0;
    for(std::string const& s : v.s)
    {
        c4::substr curr = total < buf.len ? buf.sub(total) : c4::substr();
        total += to_chars(curr, s);
    }
    return total;
}

// but if we provide the dump_() function, then the dump buffer will have only the max member size
C4_ALWAYS_INLINE void dump_(LargeType2 const& C4_RESTRICT v, c4::logns::pfn_logpump fn, c4::logns::detail::DumpBuf *buf)
{
    for(std::string const& s : v.s)
    {
        c4::catrs(buf, s);
        fn(buf->data(), buf->size());
    }
}

template<class T>
struct LargeTypeWithSizes
{
    T var;
    size_t szmax;
    size_t sztotal;
};

template<class T>
LargeTypeWithSizes<T> make_(size_t num)
{
    LargeTypeWithSizes<T> lt;
    lt.szmax = 0;
    lt.sztotal = 0;
    auto &strvec = lt.var.s;
    strvec.resize(num);
    for(size_t i = 0; i < num; ++i)
    {
        strvec[i].resize((size_t(1u) << (i + 1)) - 1, (char)('0' + i));
        strvec[i] += '\n';
        lt.szmax = std::max(strvec[i].size(), lt.szmax);
        lt.sztotal += strvec[i].size();
    }
    return lt;
}

TEST(dump, overloading_dump_uses_less_space)
{
    auto setup = SetupLogTest();

    shrink_buf();
    auto const* buf = c4::logns::detail::_dump_buf();
    size_t szbefore = buf->capacity();

    auto var_and_size = make_<LargeType2>(10);
    c4::logns::print(var_and_size.var);
    
    size_t szafter = buf->capacity();
    EXPECT_GT(szafter, szbefore);
    EXPECT_EQ(szafter, var_and_size.szmax); // because we were using powers of two
}

TEST(dump, overloading_to_chars_uses_more_space)
{
    auto setup = SetupLogTest();

    shrink_buf();
    auto const* buf = c4::logns::detail::_dump_buf();
    size_t szbefore = buf->capacity();

    auto var_and_size = make_<LargeType1>(10);
    c4::logns::print(var_and_size.var);
    
    size_t szafter = buf->capacity();
    EXPECT_GT(szafter, szbefore);
    EXPECT_EQ(szafter, var_and_size.sztotal); // because we request 2046 from 8, so the resize will pick 2046 instead of 16 (the next power of two) 
}


//------------------------------------------------------------------------

template <class... Args>
void test_print(const char* expected, Args && ...args)
{
    {
        auto h = SetupLogTest();
        c4::logns::print(std::forward<Args>(args)...);
    }
    EXPECT_STREQ(logrcv.c_str(), expected);
}

template <class T, class... Args>
void test_printsep(const char* expected, c4::logns::Sep<T> s, Args && ...args)
{
    {
        auto h = SetupLogTest();
        c4::logns::printsep(s, std::forward<Args>(args)...);
    }
    EXPECT_STREQ(logrcv.c_str(), expected);
}

template <class... Args>
void test_log(const char* expected, c4::csubstr fmt, Args && ...args)
{
    {
        auto h = SetupLogTest();
        c4::logns::log(fmt, std::forward<Args>(args)...);
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
    using namespace c4::logns;
    test_printsep("FDX\n", sep('-'), "FDX");
    test_printsep("FDX\n", sep(std::string("wtf")), "FDX");
    test_printsep("FDX-1-2-3\n", sep('-'), "FDX", 1, 2, 3);
    test_printsep("FDX---wtf---1---wtf---2---wtf---3\n", sep(std::string("---wtf---")), "FDX", 1, 2, 3);
}

TEST(log, log)
{
    test_log("the cat ate the mouse\n", "the {} ate the {}", "cat", "mouse");
}

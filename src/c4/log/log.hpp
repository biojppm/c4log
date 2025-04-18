#ifndef _c4_LOG_LOG_HPP_
#define _c4_LOG_LOG_HPP_

#include <c4/format.hpp>
#include <c4/types.hpp>
#include <c4/log/export.hpp>

/** @file log.hpp */

/** @defgroup log */

/** @defgroup log_font_styles */

namespace c4 {
namespace logns {

/** a typedef for the function pointer to the function that dumps
 * characters to the log
 * @ingroup log */
using pfn_logpump = void (*)(const char *chars, size_t num_chars);

/** get the logpump function */
pfn_logpump get_logpump();

/** set the logpump function. To reset to the default implementation, pass a null pointer. */
void set_logpump(pfn_logpump fn);

/** get a pointer to the current logpump implementation */
pfn_logpump get_logpump();


//-----------------------------------------------------------------------------

namespace detail {

#ifdef __GNUC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wtype-limits"
#endif

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 4296/*expression is always 'boolean_value'*/)
#endif

/** a minimal std::string-like zero-terminated char container, to be
 * used by dump(). We do not use std::string to save us from the
 * dependency and also from the heavy include. */
struct DumpBuf
{
    DumpBuf(DumpBuf const&) = delete;
    DumpBuf(DumpBuf &&) = delete;
    DumpBuf& operator= (DumpBuf const&) = delete;
    DumpBuf& operator= (DumpBuf &&) = delete;

    DumpBuf();
    ~DumpBuf();

    char *buf;
    size_t len;
    size_t cap;

    size_t size() const { return len; }
    size_t capacity() const { return cap; }

    void reserve(size_t sz);
    void resize(size_t sz);
    void shrink_to_fit();

          char* data()       { return buf; }
    const char* data() const { return buf; }
};

inline c4::substr to_substr(DumpBuf const& b)
{
    return {b.buf, b.len};
}

inline DumpBuf* _dump_buf()
{
    static thread_local DumpBuf buf;
    return &buf;
}

} // namespace detail

#ifdef __GNUC__
#   pragma GCC diagnostic pop
#endif

#if defined(_MSC_VER)
#   pragma warning(pop)
#   pragma warning(disable: 4296/*expression is always 'boolean_value'*/)
#endif


//-----------------------------------------------------------------------------

/** dump a single variable
 * @ingroup log
 */
template<class T>
C4_ALWAYS_INLINE void dump_(T const& C4_RESTRICT v, pfn_logpump fn, detail::DumpBuf *buf)
{
    catrs(buf, v);
    fn(buf->data(), buf->size());
}

/** dump a single variable
 * @ingroup log
 */
template <class T>
void dump(T const& C4_RESTRICT v)
{
    auto fn = get_logpump();
    C4_ASSERT(fn);
    detail::DumpBuf *buf = detail::_dump_buf();
    dump_(v, fn, buf);
}

/** dump several values without spaces between them */
template <class T, class... More>
void dump(T const& v, More const& ...args)
{
    dump(v);
    dump(args...);
}

//-----------------------------------------------------------------------------

/** terminate the variadic recursion
 @ingroup log */
template <class Arg>
inline void print_(Arg const& a)
{
    dump(a);
}

/** print multiple variables with a space separating them. No newline at the end.
 @ingroup log */
template <class Arg, class... More>
void print_(Arg const& a, More const& ...args)
{
    dump(a);
    dump(' ');
    print_(args...);
}

/** print multiple variables, with a space separating them.
 * Prints newline at the end.
 * @ingroup log */
template <class... Args>
void print(Args const& ...args)
{
    print_(args...);
    dump('\n');
}


//-----------------------------------------------------------------------------

/** @ingroup log */
template<class T>
struct Sep
{
    T const& C4_RESTRICT s;
    Sep(T const& C4_RESTRICT s_) : s(s_) {}
};

/** @ingroup log */
template<class T>
inline Sep<T> sep(T const& C4_RESTRICT s)
{
    return Sep<T>(s);
}

/** terminate the variadic recursion
 * @ingroup log */
template <class T, class Arg>
inline void printsep_(Sep<T> /*s*/, Arg const& a)
{
    dump(a);
}

/** print multiple variables with a custom separator between them.
 * No newline at the end.
 * @ingroup log */
template <class T, class Arg, class... More>
void printsep_(Sep<T> s, Arg const& a, More const& ...args)
{
    dump(a);
    dump(s.s);
    printsep_(s, args...);
}

/** print multiple variables, with a custom separator between each pair.
 * Prints newline at the end.
 * @ingroup log */
template <class T, class... Args>
void printsep(Sep<T> s, Args const& ...args)
{
    printsep_(s, args...);
    dump('\n');
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


/** log a formatted message, without printing newline.
 * terminate the variadic recursion */
inline void log_(csubstr const fmt)
{
    dump(fmt);
}

/** log a formatted message, without printing newline */
template <class Arg, class... More>
void log_(csubstr const fmt, Arg const& a, More const& ...args)
{
    auto pos = fmt.find("{}");
    if(pos == csubstr::npos)
    {
        dump(fmt);
    }
    else
    {
        dump(fmt.sub(0, pos));
        dump(a);
        log_(fmt.sub(pos+2), args...);
    }
}

/** log a formatted message. For example:
@begincode
log("the {} ate the {}", "cat", "mouse");
@endcode
@ingroup log */
template <class... Args>
void log(csubstr const fmt, Args const& ...args)
{
    log_(fmt, args...);
    dump('\n');
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


C4_SUPPRESS_WARNING_GCC_PUSH
#if __GNUC__ > 5
C4_SUPPRESS_WARNING_GCC("-Wunused-const-variable")
#endif

// https://misc.flogisoft.com/bash/tip_colors_and_formatting

/** foreground terminal colors
 * @ingroup log_font_styles */
constexpr const char fg_reset        [] = "\033[0m";
constexpr const char fg_black        [] = "\033[30m";
constexpr const char fg_red          [] = "\033[31m";
constexpr const char fg_green        [] = "\033[32m";
constexpr const char fg_yellow       [] = "\033[33m";
constexpr const char fg_blue         [] = "\033[34m";
constexpr const char fg_magenta      [] = "\033[35m";
constexpr const char fg_cyan         [] = "\033[36m";
constexpr const char fg_light_gray   [] = "\033[37m";
constexpr const char fg_dark_gray    [] = "\033[90m";
constexpr const char fg_light_red    [] = "\033[91m";
constexpr const char fg_light_green  [] = "\033[92m";
constexpr const char fg_light_yellow [] = "\033[93m";
constexpr const char fg_light_blue   [] = "\033[94m";
constexpr const char fg_light_magenta[] = "\033[95m";
constexpr const char fg_light_cyan   [] = "\033[96m";
constexpr const char fg_white        [] = "\033[97m";

/** background terminal colors
 * @ingroup log_font_styles */
constexpr const char bg_reset        [] = "\033[49m";
constexpr const char bg_black        [] = "\033[40m";
constexpr const char bg_red          [] = "\033[41m";
constexpr const char bg_green        [] = "\033[42m";
constexpr const char bg_yellow       [] = "\033[43m";
constexpr const char bg_blue         [] = "\033[44m";
constexpr const char bg_magenta      [] = "\033[45m";
constexpr const char bg_cyan         [] = "\033[46m";
constexpr const char bg_light_gray   [] = "\033[47m";
constexpr const char bg_dark_gray    [] = "\033[100m";
constexpr const char bg_light_red    [] = "\033[101m";
constexpr const char bg_light_green  [] = "\033[102m";
constexpr const char bg_light_yellow [] = "\033[103m";
constexpr const char bg_light_blue   [] = "\033[104m";
constexpr const char bg_light_magenta[] = "\033[105m";
constexpr const char bg_light_cyan   [] = "\033[106m";
constexpr const char bg_white        [] = "\033[107m";

/** term font styles
 * @ingroup log_font_styles */
constexpr const char st_bold            [] = "\033[1m";
constexpr const char st_bold_reset      [] = "\033[21m";
constexpr const char st_dim             [] = "\033[2m";
constexpr const char st_dim_reset       [] = "\033[22m";
constexpr const char st_underlined      [] = "\033[4m";
constexpr const char st_underlined_reset[] = "\033[24m";
constexpr const char st_blink           [] = "\033[5m";
constexpr const char st_blink_reset     [] = "\033[25m";
constexpr const char st_reverse         [] = "\033[7m";
constexpr const char st_reverse_reset   [] = "\033[27m";
constexpr const char st_hidden          [] = "\033[8m";
constexpr const char st_hidden_reset    [] = "\033[28m";

C4_SUPPRESS_WARNING_GCC_POP

} // namespace logns

using logns::dump;
using logns::print;
using logns::printsep;
using logns::log;
using logns::print_;
using logns::printsep_;
using logns::log_;

using logns::set_logpump;
using logns::get_logpump;

} // namespace c4

#endif // _c4_LOG_LOG_HPP_

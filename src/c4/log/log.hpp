#ifndef _c4_LOG_LOG_HPP_
#define _c4_LOG_LOG_HPP_

#include <c4/format.hpp>
#include <c4/log/export.hpp>

/** @file log.hpp */

/** @defgroup log */

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
    C4_NO_COPY_OR_MOVE(DumpBuf);

    DumpBuf();
    ~DumpBuf();

    char *buf;
    size_t len;
    size_t cap;

    size_t size() const { return len; }
    size_t capacity() const { return cap; }

    void resize(size_t sz);

          char& operator[] (size_t i)       { C4_ASSERT(i >= 0 && i < len); return *(buf + i); }
    const char& operator[] (size_t i) const { C4_ASSERT(i >= 0 && i < len); return *(buf + i); }

          char* data()       { return buf; }
    const char* data() const { return buf; }

    const char* c_str() const { return buf; }
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
template <class T>
void dump(T const& v)
{
    auto fn = get_logpump();
    C4_ASSERT(fn);
    detail::DumpBuf *buf = detail::_dump_buf();
    c4::catrs(buf, v);
    fn(buf->c_str(), buf->size());
}

/** dump several values without spaces between them */
template <class T, class... More>
void dump(T const& v, More const& ...args)
{
    dump(v);
    dump(args...);
}

//-----------------------------------------------------------------------------

/** @ingroup log */
struct Sep
{
    char c;
    Sep(char c_) : c(c_) {}
};

inline Sep sep(char c)
{
    return Sep(c);
}


// terminate the recursion
template <class Arg>
inline void _print(Arg const& a)
{
    dump(a);
}

template <class Arg, class... More>
void _print(Arg const& a, More const& ...args)
{
    dump(a);
    dump(' ');
    _print(args...);
}

// terminate the recursion
template <class Arg>
inline void _printsep(Sep /*s*/, Arg const& a)
{
    dump(a);
}

template <class Arg, class... More>
void _printsep(Sep s, Arg const& a, More const& ...args)
{
    dump(a);
    if(s.c) dump(s.c);
    _printsep(s, args...);
}


/** print multiple variables, with a space separating them. Prints
    newline at the end.
    @ingroup log */
template <class... Args>
void print(Args const& ...args)
{
    _print(args...);
    dump('\n');
}

/** print multiple variables, with a custom character separating
    them. Prints newline at the end.
    @ingroup log */
template <class... Args>
void printsep(Sep s, Args const& ...args)
{
    _printsep(s, args...);
    dump('\n');
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/// terminate the recursion
inline void _log(csubstr const fmt)
{
    dump(fmt);
}

/** log a formatted message, without printing newline */
template <class Arg, class... More>
void _log(csubstr const fmt, Arg const& a, More const& ...args)
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
        _log(fmt.sub(pos+2), args...);
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
    _log(fmt, args...);
    dump('\n');
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/** @defgroup termcolor */

// https://misc.flogisoft.com/bash/tip_colors_and_formatting

/** term colors
 * @ingroup termcolor */
constexpr const char bold            [] = "\033[1m";
constexpr const char bold_reset      [] = "\033[21m";
constexpr const char dim             [] = "\033[2m";
constexpr const char dim_reset       [] = "\033[22m";
constexpr const char underlined      [] = "\033[4m";
constexpr const char underlined_reset[] = "\033[24m";
constexpr const char blink           [] = "\033[5m";
constexpr const char blink_reset     [] = "\033[25m";
constexpr const char reverse         [] = "\033[7m";
constexpr const char reverse_reset   [] = "\033[27m";
constexpr const char hidden          [] = "\033[8m";
constexpr const char hidden_reset    [] = "\033[28m";

/** foreground terminal colors
 * @ingroup termcolor */
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
 * @ingroup termcolor */
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


} // namespace logns

using logns::_log;
using logns::log;
using logns::_print;
using logns::print;
using logns::dump;
using logns::set_logpump;
using logns::get_logpump;

} // namespace c4

#endif // _c4_LOG_LOG_HPP_

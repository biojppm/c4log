#ifndef _c4_LOG_LOG_HPP_
#define _c4_LOG_LOG_HPP_

#include <c4/to_str.hpp>

/** @file log.hpp */

/** @defgroup log */

namespace c4 {
namespace log {

/** a typedef for the function pointer to the function that dumps
 * characters to the log
 * @ingroup log */
using pfn_logpump = void (*)(const char *c_str, size_t len);

namespace detail {
extern pfn_logpump logpump;
} // namespace detail

/** set the logpump function. To reset to the default implementation, pass a null pointer. */
void set_logpump(pfn_logpump fn);

/** get a pointer to the current logpump implementation */
pfn_logpump get_logpump();


//-----------------------------------------------------------------------------

namespace detail {

/** a minimal std::string-like zero-terminated char container, to be
 * used by printvar(). We do not use std::string to save us from the
 * dependency and also from the heavy include. */
struct PrintVarBuf
{
    C4_NO_COPY_OR_MOVE(PrintVarBuf);

    PrintVarBuf();
    ~PrintVarBuf();

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

inline c4::substr to_substr(PrintVarBuf const& b)
{
    return {b.buf, b.len};
}

inline PrintVarBuf* _printvar_buf()
{
    static thread_local PrintVarBuf buf;
    return &buf;
}

} // namespace detail


/** log a single variable
 * @ingroup log
 */
template< class T >
void printvar(T const& v)
{
    auto &buf = * detail::_printvar_buf();
    c4::catrs(&buf, v);
    detail::logpump(buf.c_str(), buf.size());
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

namespace detail {

// terminate the recursion
template< class Arg >
inline void _print(Arg const& a)
{
    printvar(a);
}

template< class Arg, class... More >
void _print(Arg const& a, More const& ...args)
{
    printvar(a);
    printvar(' ');
    _print(args...);
}

// terminate the recursion
template< class Arg >
inline void _printsep(Sep s, Arg const& a)
{
    printvar(a);
}

template< class Arg, class... More >
void _printsep(Sep s, Arg const& a, More const& ...args)
{
    printvar(a);
    printvar(s.c);
    _printsep(s, args...);
}

} // namespace detail


/** print multiple variables, with a space separating them. Prints
    newline at the end.
    @ingroup log */
template< class... Args >
void print(Args const& ...args)
{
    detail::_print(args...);
    printvar('\n');
}

/** print multiple variables, with a custom character separating
    them. Prints newline at the end.
    @ingroup log */
template< class... Args >
void printsep(Sep s, Args const& ...args)
{
    detail::_printsep(s, args...);
    printvar('\n');
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

namespace detail {

/// terminate the recursion
inline void _log(csubstr const fmt)
{
    printvar(fmt);
}

template< class Arg, class... More >
void _log(csubstr const fmt, Arg const& a, More const& ...args)
{
    auto pos = fmt.find("{}");
    if(pos == csubstr::npos)
    {
        printvar(fmt);
    }
    else
    {
        printvar(fmt.sub(0, pos));
        printvar(a);
        _log(fmt.sub(pos+2), args...);
    }
}

} // namespace detail


/** log a formatted message. For example:
@begincode
log("the {} ate the {}", "cat", "mouse");
@endcode
@ingroup log */
template< class... Args >
void log(csubstr const fmt, Args const& ...args)
{
    detail::_log(fmt, args...);
    printvar('\n');
}

} // namespace log
} // namespace c4

#endif // _c4_LOG_LOG_HPP_

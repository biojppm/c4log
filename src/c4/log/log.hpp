#ifndef _c4_LOG_LOG_HPP_
#define _c4_LOG_LOG_HPP_

#include <c4/format.hpp>

/** @file log.hpp */

/** @defgroup log */

namespace c4 {
namespace logns {

/** a typedef for the function pointer to the function that dumps
 * characters to the log
 * @ingroup log */
using pfn_logpump = void (*)(const char *chars, size_t num_chars);

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


//-----------------------------------------------------------------------------

/** dump a single variable
 * @ingroup log
 */
template <class T>
void dump(T const& v)
{
    detail::DumpBuf *buf = detail::_dump_buf();
    c4::catrs(buf, v);
    detail::logpump(buf->c_str(), buf->size());
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
inline void _printsep(Sep s, Arg const& a)
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

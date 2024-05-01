#include "c4/log/log.hpp"
#include "c4/memory_resource.hpp"

#include <stdio.h>
#include <cstddef>

#ifdef C4_WIN
#   include "c4/windows_push.hpp"
#   include <windef.h>
#   include <debugapi.h>
#   include "c4/windows_pop.hpp"
#endif

C4_SUPPRESS_WARNING_GCC_CLANG_WITH_PUSH("-Wold-style-cast")

namespace c4 {
namespace logns {

C4_SUPPRESS_WARNING_GCC_CLANG_WITH_PUSH("-Wold-style-cast")

namespace detail {

/** a default implementation for the logpump function */
void _logpump_impl(const char *chars, size_t num_chars)
{
    fwrite(chars, 1, num_chars, stdout);
#ifdef _MSC_VER
    OutputDebugStringA(chars);
#endif
}

C4LOG_EXPORT pfn_logpump logpump = &_logpump_impl;

} // namespace detail

void set_logpump(pfn_logpump fn)
{
    detail::logpump = fn ? fn : &detail::_logpump_impl;
}

pfn_logpump get_logpump()
{
    return detail::logpump;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

namespace detail {

DumpBuf::~DumpBuf()
{
    c4::afree(buf);
    buf = nullptr;
}

DumpBuf::DumpBuf()
    :
    buf((char*) c4::aalloc(32, alignof(std::max_align_t))),
    len(0),
    cap(32)
{
    memset(buf, 0, cap);
}

void DumpBuf::reserve(size_t ncap)
{
    if(ncap <= cap) return;
    char *tmp = (char*) c4::aalloc(ncap, alignof(std::max_align_t));
    memcpy(tmp, buf, len);
    c4::afree(buf);
    buf = tmp;
    cap = ncap;
}

void DumpBuf::resize(size_t sz)
{
    if(sz <= cap)
    {
        len = sz;
        return;
    }
    // double the capacity or use the requested size if double is not enough
    reserve(cap*2 >= sz ? cap*2 : sz);
    len = sz;
}

void DumpBuf::shrink_to_fit()
{
    char *tmp = (char*) c4::aalloc(len, alignof(std::max_align_t));
    memcpy(tmp, buf, len);
    c4::afree(buf);
    buf = tmp;
    cap = len;
}

} // namespace detail

C4_SUPPRESS_WARNING_GCC_CLANG_POP

} // namespace logns
} // namespace c4

C4_SUPPRESS_WARNING_GCC_CLANG_POP

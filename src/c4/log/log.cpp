#include "c4/log/log.hpp"
#include "c4/log/color.hpp"
#include "c4/memory_resource.hpp"

#include <stdio.h>
#include <cstddef>

#ifdef C4_WIN
#   include "c4/windows_push.hpp"
#   include <windef.h>
#   include <debugapi.h>
#endif

namespace c4 {
namespace logns {

thread_local const char* ScopedTermColor::curr = "";

namespace detail {

/** a default implementation for the logpump function */
void _logpump_impl(const char *chars, size_t num_chars)
{
    fwrite(chars, 1, num_chars, stdout);
#ifdef _MSC_VER
    OutputDebugStringA(chars);
#endif
}

pfn_logpump logpump = &_logpump_impl;

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

void DumpBuf::resize(size_t sz)
{
    if(sz < cap)
    {
        memset(buf+sz, 0, cap-sz);
        len = sz;
        return;
    }

    // double the capacity or use the size if double is not enough
    size_t ncap = cap*2 > sz+1 ? cap*2 : sz+1;

    char *tmp = (char*) c4::aalloc(ncap, alignof(std::max_align_t));
    memcpy(tmp, buf, cap);
    memset(tmp+cap, 0, ncap-cap);
    c4::afree(buf);
    buf = tmp;
    cap = ncap;
    len = sz;
}

} // namespace detail

} // namespace logns
} // namespace c4

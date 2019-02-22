#ifndef _c4_LOG_COLOR_HPP_
#define _c4_LOG_COLOR_HPP_

#include "c4/log/log.hpp"

/** @file color.hpp */

/** @defgroup termcolor */

namespace c4 {
namespace logns {

/** a RAII-style class that sets the terminal color
 * @ingroup termcolor */
struct ScopedTermColor
{
    static thread_local const char* curr;
    const char* prev;

    template< size_t N >
    ScopedTermColor(const char (&c)[N])
    {
        prev = curr;
        printvar(c);
    }
    ScopedTermColor()
    {
        dump(prev);
        curr = prev;
    }
};

using ccptr = const char * const;

// https://misc.flogisoft.com/bash/tip_colors_and_formatting

/** term colors
 * @ingroup termcolor */
constexpr ccptr bold             = "\033[1m",
                bold_reset       = "\033[21m",
                dim              = "\033[2m",
                dim_reset        = "\033[22m",
                underlined       = "\033[4m",
                underlined_reset = "\033[24m",
                blink            = "\033[5m",
                blink_reset      = "\033[25m",
                reverse          = "\033[7m",
                reverse_reset    = "\033[27m",
                hidden           = "\033[8m",
                hidden_reset     = "\033[28m";

/** foreground terminal colors
 * @ingroup termcolor */
constexpr ccptr fg_reset         = "\033[0m",
                fg_black         = "\033[30m",
                fg_red           = "\033[31m",
                fg_green         = "\033[32m",
                fg_yellow        = "\033[33m",
                fg_blue          = "\033[34m",
                fg_magenta       = "\033[35m",
                fg_cyan          = "\033[36m",
                fg_light_gray    = "\033[37m",
                fg_dark_gray     = "\033[90m",
                fg_light_red     = "\033[91m",
                fg_light_green   = "\033[92m",
                fg_light_yellow  = "\033[93m",
                fg_light_blue    = "\033[94m",
                fg_light_magenta = "\033[95m",
                fg_light_cyan    = "\033[96m",
                fg_white         = "\033[97m";

/** background terminal colors
 * @ingroup termcolor */
constexpr ccptr bg_reset         = "\033[49m",
                bg_black         = "\033[40m",
                bg_red           = "\033[41m",
                bg_green         = "\033[42m",
                bg_yellow        = "\033[43m",
                bg_blue          = "\033[44m",
                bg_magenta       = "\033[45m",
                bg_cyan          = "\033[46m",
                bg_light_gray    = "\033[47m",
                bg_dark_gray     = "\033[100m",
                bg_light_red     = "\033[101m",
                bg_light_green   = "\033[102m",
                bg_light_yellow  = "\033[103m",
                bg_light_blue    = "\033[104m",
                bg_light_magenta = "\033[105m",
                bg_light_cyan    = "\033[106m",
                bg_white         = "\033[107m";

} // namespace logns
} // namespace c4


#endif /* _c4_LOG_COLOR_HPP_ */

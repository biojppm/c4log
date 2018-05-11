set(src
    c4/log/log.hpp
    c4/log/log.cpp
    c4/log/color.hpp
)

c4_require_module(c4log
    c4core
    REMOTE GIT_REPOSITORY https://github.com/biojppm/c4core GIT_TAG master
    )

c4_add_library(C4LOG c4log
    SOURCES ${src}
    LIBS c4core
    INC_DIRS ${CMAKE_CURRENT_LIST_DIR}
)

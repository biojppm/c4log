cmake_minimum_required(VERSION 3.2)
project(c4log)

include(./ext/c4core/cmake/c4Project.cmake)

c4_declare_project(c4log
    DESC "C++ logging utilities"
    AUTHOR "Joao Paulo Magalhaes <dev@jpmag.me>"
    MAJOR 0 MINOR 0 RELEASE 1)

set(C4LOG_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
set(C4LOG_EXT_DIR ${CMAKE_CURRENT_LIST_DIR}/ext)

c4_require_subproject(c4core
    SUBDIRECTORY ${C4LOG_EXT_DIR}/c4core)

c4_add_library(c4log
    SOURCE_ROOT ${C4LOG_SRC_DIR}
    SOURCES
      c4/log/log.hpp
      c4/log/log.cpp
      c4/log/color.hpp
    LIBS c4core
    INC_DIRS
       $<BUILD_INTERFACE:${C4LOG_SRC_DIR}> $<INSTALL_INTERFACE:include>
    )

c4_install_target(c4log)
c4_install_exports()

if(C4LOG_BUILD_TESTS)
    enable_testing() # this must be done here (and not inside the test dir)
                     # so that the test targets are available at the top level
    add_subdirectory(test)
endif()

if(C4LOG_BUILD_BENCHMARKS)
    add_subdirectory(bm)
endif()

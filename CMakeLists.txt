cmake_minimum_required(VERSION 3.12)
include(./ext/c4core/cmake/c4Project.cmake)
project(c4log
    DESCRIPTION "C++ logging utilities"
    HOMEPAGE_URL "https://github.com/biojppm/c4log/")
c4_project(VERSION 0.0.1
    AUTHOR "Joao Paulo Magalhaes <dev@jpmag.me>")

c4_require_subproject(c4core
    SUBDIRECTORY ${C4LOG_EXT_DIR}/c4core)

c4_add_library(c4log
    SOURCE_ROOT ${C4LOG_SRC_DIR}
    SOURCES
      c4/log/export.hpp
      c4/log/log.hpp
      c4/log/log.cpp
    LIBS c4core
    INC_DIRS
       $<BUILD_INTERFACE:${C4LOG_SRC_DIR}> $<INSTALL_INTERFACE:include>
    )

c4_install_target(c4log)
c4_install_exports()

c4_add_dev_targets()

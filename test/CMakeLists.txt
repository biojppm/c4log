c4_setup_testing(C4LOG ON)

function(c4log_add_test name)
    c4_add_executable(C4LOG c4log-test-${name}
        SOURCES ${ARGN} main.cpp
        INC_DIRS ${CMAKE_CURRENT_LIST_DIR}
        LIBS c4log gtest
        FOLDER c4log-test)
    c4_add_test(C4LOG c4log-test-${name} ON)
endfunction(c4log_add_test)

c4log_add_test(basic test.cpp)

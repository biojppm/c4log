c4_setup_testing()

function(c4log_add_test name)
    c4_add_executable(c4log-test-${name}
        SOURCES ${ARGN}
        INC_DIRS ${CMAKE_CURRENT_LIST_DIR}
        LIBS c4log doctest
        FOLDER test)
    c4_add_test(c4log-test-${name})
endfunction(c4log_add_test)

c4log_add_test(basic test.cpp)

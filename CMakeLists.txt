cmake_minimum_required(VERSION 4.2)
project(cttt4by4 C)
enable_testing()

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(flags INTERFACE)
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
target_compile_options(flags INTERFACE
        -Wall -Wextra -Wpedantic
                -fsanitize=address,undefined
                -fno-omit-frame-pointer
#)
        -g)
target_link_options(flags INTERFACE -fsanitize=address,undefined)
elseif (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    target_compile_options(flags INTERFACE
            -Wall -Wextra -Wpedantic -O3
            )
endif ()

add_executable(cttt4by4 main.c)
add_executable(hashtable_test tests/hashtable_test.c)
add_executable(board_test tests/board_test.c)
add_executable(speed_test tests/speed_test.c)
add_executable(solve_test tests/solve_test.c)
target_link_libraries(cttt4by4 PRIVATE flags)
target_link_libraries(hashtable_test PRIVATE flags)
target_link_libraries(solve_test PRIVATE flags)
target_link_libraries(board_test PRIVATE flags)

add_test(NAME hashtable_test COMMAND hashtable_test)
add_test(NAME board_test COMMAND board_test)
add_test(NAME solve_test COMMAND solve_test)
add_test(NAME speed_test COMMAND speed_test)

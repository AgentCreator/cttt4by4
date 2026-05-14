cmake_minimum_required(VERSION 4.2)
project(cttt4by4 C)
enable_testing()

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(flags INTERFACE)
target_compile_options(flags INTERFACE
        -Wall -Wextra -Wpedantic -O3
#        -fsanitize=address,undefined
#        -fno-omit-frame-pointer
)
#        -g)
#target_link_options(flags INTERFACE -fsanitize=address,undefined)

add_executable(cttt4by4 main.c )
add_executable(hashtable_test tests/hashtable_test.c)
add_executable(board_test tests/board_test.c)
target_link_libraries(cttt4by4 PRIVATE flags)
target_link_libraries(hashtable_test PRIVATE flags)
target_link_libraries(board_test PRIVATE flags)

add_test(NAME hashtable_test COMMAND hashtable_test)
add_test(NAME board_test COMMAND board_test)

cmake_minimum_required(VERSION 3.24)
project(lcd_i2c
        VERSION 1.2.0)

add_library(lcd_i2c STATIC
    src/lcd_i2c.c
)

target_include_directories(lcd_i2c PUBLIC
    inc/
    ${CMAKE_CURRENT_BINARY_DIR}/inc
)
find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}" "-header-filter=.*,--extra-arg=--target=arm")
set_target_properties(lcd_i2c PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")

target_compile_options(lcd_i2c PRIVATE
    -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -fms-extensions 
)
set_property(TARGET lcd_i2c PROPERTY C_STANDARD 23)
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/inc/lcd_i2c/version.h"
)

# Minimum version requirement for CMake
cmake_minimum_required(VERSION 3.5)

# Project name and language
project(MyProject C)

# Specify the directories where the header files are
include_directories(include)

# Add executable target with source files
add_executable(main src/main.c src/help_functions.c) # <-- Add help_functions.c here

# Include clang-format as custom target
find_program(CLANG_FORMAT "clang-format")
if(CLANG_FORMAT)
    add_custom_target(
        clang-format
        COMMAND ${CLANG_FORMAT}
        -i
        -style=file
        ${CMAKE_SOURCE_DIR}/src/*.c
    )
endif()

# Find cppcheck
find_program(CPPCHECK_EXECUTABLE NAMES cppcheck)
if(NOT CPPCHECK_EXECUTABLE)
    message(WARNING "cppcheck not found!")
endif()

# Create custom target for cppcheck
if(CPPCHECK_EXECUTABLE)
    add_custom_target(
        cppcheck
        COMMAND ${CPPCHECK_EXECUTABLE}
        --enable=warning,style,performance,portability,unusedFunction
        --std=c11
        --template=gcc
        --verbose
        --quiet
        ${CMAKE_SOURCE_DIR}/src/*.c
    )
endif()

# Output message
message(STATUS "--------------------------------------------------")
message(STATUS "To run both tasks, simply execute ./main.")
message(STATUS "To format the code, run: make clang-format.")
message(STATUS "--------------------------------------------------")

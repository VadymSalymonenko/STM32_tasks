cmake_minimum_required(VERSION 3.5)
project(MyProject C)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/path/to/your/stm32_gcc.cmake)

# Вказуємо шлях до драйверів та CMSIS
set(HAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/drivers/STM32G0xx_HAL_Driver)
set(CMSIS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/drivers/CMSIS)

# Компоненти, які вам потрібні
set(HAL_COMP_LIST RCC GPIO CORTEX)
set(CMSIS_COMP_LIST STM32G0)

find_package(CMSIS COMPONENTS "${CMSIS_COMP_LIST}" REQUIRED)
find_package(HAL COMPONENTS "${HAL_COMP_LIST}" REQUIRED)

# Вказуємо додаткові шляхи до заголовочних файлів
include_directories(include ${HAL_DIR}/Inc ${CMSIS_DIR}/Include examples/blink/include)

# Змінюємо список джерел
file(GLOB_RECURSE SOURCES "src/*.c" "examples/blink/src/blink.c")

add_executable(main ${SOURCES})

target_link_libraries(main
    HAL::STM32::G0::RCC
    HAL::STM32::G0::GPIO
    HAL::STM32::G0::CORTEX
    CMSIS::STM32::G0
    STM32::NoSys
)

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
        ${CMAKE_SOURCE_DIR}/drivers/stm32g0xx_hal/Src/*.c # <-- Add HAL src here
    )
endif()

# Output message
message(STATUS "--------------------------------------------------")
message(STATUS "To run both tasks, simply execute ./main.")
message(STATUS "To format the code, run: make clang-format.")
message(STATUS "--------------------------------------------------")

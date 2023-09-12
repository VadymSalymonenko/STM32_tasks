cmake_minimum_required(VERSION 3.16)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/stm32_gcc.cmake)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../cmake")

# Project and languages
project(button_test LANGUAGES C ASM)
add_compile_options($<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>)

# Options
option(BUTTON_TEST_G0_EXAMPLE "Compile G0 example" ON)

file(GLOB SOURCES "src/*.c")
file(GLOB HEADERS "inc/*.h")

# Set default components
set(HAL_COMP_LIST RCC GPIO CORTEX)
set(CMSIS_COMP_LIST "")

# Configuration for BUTTON_TEST_G0_EXAMPLE
if(BUTTON_TEST_G0_EXAMPLE)
    list(APPEND CMSIS_COMP_LIST STM32G0)
    list(APPEND HAL_COMP_LIST STM32G0)
endif()

# Packages
find_package(CMSIS COMPONENTS ${CMSIS_COMP_LIST} REQUIRED)
find_package(HAL COMPONENTS ${HAL_COMP_LIST} REQUIRED)

# ThreadX Configuration
set(THREADX_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../drivers/ThreadX")

# Main executable
if(BUTTON_TEST_G0_EXAMPLE)
    add_executable(button_test-g0 ${SOURCES} tx_initialize_low_level.S)

    target_include_directories(button_test-g0 PRIVATE inc)
    
    target_sources(button_test-g0 PRIVATE
        /opt/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_uart.c
        /opt/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_uart_ex.c
        /opt/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_pwr_ex.c
    )
    
    target_include_directories(button_test-g0 PRIVATE
            ${THREADX_INCLUDE_DIR}/common/inc
            ${THREADX_INCLUDE_DIR}/ports/cortex_m0/gnu/inc
    )

    target_link_libraries(button_test-g0
            HAL::STM32::G0::RCC
            HAL::STM32::G0::GPIO
            HAL::STM32::G0::CORTEX
            CMSIS::STM32::G030F6
            STM32::NoSys
            ${THREADX_INCLUDE_DIR}/build/libthreadx.a
    )

    stm32_print_size_of_target(button_test-g0)
endif()

# Custom targets
add_custom_target(compile
        COMMAND ${CMAKE_MAKE_PROGRAM}
        COMMENT "Compiling the project as ELF"
)

add_custom_target(load_to_flash
        DEPENDS button_test-g0
        COMMAND arm-none-eabi-objcopy -O binary button_test-g0.elf button_test-g0.bin
        COMMAND st-flash write button_test-g0.bin 0x8000000
        COMMENT "Converting ELF to BIN and flashing to the board"
)

add_custom_target(compile_load
        COMMAND ${CMAKE_MAKE_PROGRAM}
        COMMAND arm-none-eabi-objcopy -O binary button_test-g0.elf button_test-g0.bin
        COMMAND st-flash write button_test-g0.bin 0x8000000
        COMMENT "Compiling and flashing the board"
)

add_dependencies(load_to_flash compile)
add_dependencies(compile_load compile)

# Default target
add_custom_target(custom_default
        COMMAND ${CMAKE_COMMAND} -E echo "please, choose option: compile, load_to_flash, or compile_load"
)

file(APPEND ${CMAKE_BINARY_DIR}/Makefile "\n.DEFAULT_GOAL := custom_default\n")

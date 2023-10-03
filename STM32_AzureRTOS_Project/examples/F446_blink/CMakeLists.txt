cmake_minimum_required(VERSION 3.16)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/stm32_gcc.cmake)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=soft")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfloat-abi=soft")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../cmake")


project(stm32-blinky LANGUAGES C ASM)
add_compile_options($<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>)

file(GLOB SOURCES "src/*.c")
file(GLOB HEADERS "inc/*.h")

# Set default components for STM32F4
set(HAL_COMP_LIST STM32F4 RCC GPIO CORTEX)
set(CMSIS_COMP_LIST STM32F4)

# Packages
find_package(CMSIS COMPONENTS ${CMSIS_COMP_LIST} REQUIRED)
find_package(HAL COMPONENTS ${HAL_COMP_LIST} REQUIRED)

# ThreadX Configuration
set(THREADX_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../drivers/ThreadX")

# Main executable for STM32F446RE
add_executable(stm32-blinky-f4 ${SOURCES} tx_initialize_low_level.S)

target_include_directories(stm32-blinky-f4 PRIVATE inc)
target_compile_options(stm32-blinky-f4 PRIVATE "-mfloat-abi=soft")

target_sources(stm32-blinky-f4 PRIVATE
    /opt/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c
)

target_include_directories(stm32-blinky-f4 PRIVATE
    ${THREADX_INCLUDE_DIR}/common/inc
    ${THREADX_INCLUDE_DIR}/ports/cortex_m4/gnu/inc  # Changed from m0 to m4
)

target_link_libraries(stm32-blinky-f4
    HAL::STM32::F4::RCC
    HAL::STM32::F4::GPIO
    HAL::STM32::F4::CORTEX
    CMSIS::STM32::F446RE   # Specific to the STM32F446RE
    STM32::NoSys
    ${THREADX_INCLUDE_DIR}/build/libthreadx.a
)
stm32_print_size_of_target(stm32-blinky-f4)

# Custom targets
add_custom_target(compile
        COMMAND ${CMAKE_MAKE_PROGRAM}
        COMMENT "Compiling the project as ELF"
)

add_custom_target(load_to_flash
        DEPENDS stm32-blinky-f4
        COMMAND arm-none-eabi-objcopy -O binary stm32-blinky-f4.elf stm32-blinky-f4.bin
        COMMAND st-flash write stm32-blinky-f4.bin 0x8000000
        COMMENT "Converting ELF to BIN and flashing to the board"
)

add_custom_target(compile_load
        COMMAND ${CMAKE_MAKE_PROGRAM}
        COMMAND arm-none-eabi-objcopy -O binary stm32-blinky-f4.elf stm32-blinky-f4.bin
        COMMAND st-flash write stm32-blinky-f4.bin 0x8000000
        COMMENT "Compiling and flashing the board"
)

add_dependencies(load_to_flash compile)
add_dependencies(compile_load compile)

# Default target
add_custom_target(custom_default
        COMMAND ${CMAKE_COMMAND} -E echo "please, choose option: compile, load_to_flash, or compile_load"
)

file(APPEND ${CMAKE_BINARY_DIR}/Makefile "\n.DEFAULT_GOAL := custom_default\n")

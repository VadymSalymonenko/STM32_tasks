cmake_minimum_required(VERSION 3.16)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/stm32_gcc.cmake)

add_compile_options($<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>)


option(BLINKY_G0_EXAMPLE "Compile G0 example" ON)
option(USE_CPP_FILE "Use the .cpp example file instead of the .c one" OFF)

project(stm32-blinky C ASM)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../cmake")

set(MAIN_SOURCE_FILE main.c)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
set(HAL_COMP_LIST RCC GPIO CORTEX)
set(CMSIS_COMP_LIST "")

if(BLINKY_G0_EXAMPLE)
    list(APPEND CMSIS_COMP_LIST STM32G0)
    list(APPEND HAL_COMP_LIST STM32G0)
endif()

find_package(CMSIS COMPONENTS "${CMSIS_COMP_LIST}" REQUIRED)
find_package(HAL COMPONENTS "${HAL_COMP_LIST}" REQUIRED)

set(THREADX_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../drivers/ThreadX/build/libthreadx.a")
#set(THREADX_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../drivers/ThreadX/tx.a")


if(BLINKY_G0_EXAMPLE)
    add_executable(stm32-blinky-g0 main.c stm32g0xx_hal_conf.h)
    
    target_include_directories(stm32-blinky-g0 PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/../../drivers/ThreadX/common/inc"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../drivers/ThreadX/ports/cortex_m0/gnu/inc"
    )
   # target_sources(stm32-blinky-g0
    #    PRIVATE
    #    ${CMAKE_CURRENT_SOURCE_DIR}/../../drivers/ThreadX/ports/cortex_m0/gnu/example_build/sample_threadx.c
    #)
    target_link_libraries(stm32-blinky-g0
        HAL::STM32::G0::RCC
        HAL::STM32::G0::GPIO
        HAL::STM32::G0::CORTEX
        CMSIS::STM32::G030F6
        STM32::NoSys
        ${THREADX_LIB_PATH}
    )
    stm32_print_size_of_target(stm32-blinky-g0)
endif()


# Custom Targets
add_custom_target(compile
    COMMAND ${CMAKE_MAKE_PROGRAM}
    COMMENT "Compiling the project as ELF"
)

add_custom_target(load_to_flash
    COMMAND arm-none-eabi-objcopy -O binary stm32-blinky-g0.elf stm32-blinky-g0.bin
    COMMAND st-flash write stm32-blinky-g0.bin 0x8000000
    COMMENT "Converting ELF to BIN and flashing to the board"
    DEPENDS stm32-blinky-g0
)

add_custom_target(compile_load
    COMMAND ${CMAKE_MAKE_PROGRAM}
    COMMAND arm-none-eabi-objcopy -O binary stm32-blinky-g0.elf stm32-blinky-g0.bin
    COMMAND st-flash write stm32-blinky-g0.bin 0x8000000
    COMMENT "Compiling and flashing the board"
)

add_dependencies(load_to_flash compile)
add_dependencies(compile_load compile)

# Default target displaying message only
add_custom_target(custom_default
    COMMAND ${CMAKE_COMMAND} -E echo "please, choose option: compile, load_to_flash, or compile_load"
)

file(APPEND ${CMAKE_BINARY_DIR}/Makefile "\n.DEFAULT_GOAL := custom_default\n")

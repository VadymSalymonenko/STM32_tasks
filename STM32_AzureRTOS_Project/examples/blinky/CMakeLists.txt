cmake_minimum_required(VERSION 3.16)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/stm32_gcc.cmake)

# Configure here which STM32 target(s) to build
option(BLINKY_G0_EXAMPLE "Compile G0 example" ON) # New option for G0


option(USE_CPP_FILE "Use the .cpp example file instead of the .c one" OFF)

if(USE_CPP_FILE)
    project(stm32-blinky C CXX ASM)
    set(MAIN_SOURCE_FILE blinky.cpp)
else()
    project(stm32-blinky C ASM)
    set(MAIN_SOURCE_FILE blinky.c)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)


set(HAL_COMP_LIST RCC GPIO CORTEX)
set(CMSIS_COMP_LIST "")

if(BLINKY_G0_EXAMPLE)  # New block for G0
    list(APPEND CMSIS_COMP_LIST STM32G0)
    list(APPEND HAL_COMP_LIST STM32G0)
endif()

find_package(CMSIS COMPONENTS "${CMSIS_COMP_LIST}" REQUIRED)
find_package(HAL COMPONENTS "${HAL_COMP_LIST}" REQUIRED)

# STM32G030F6P6-Discovery (New Executable)
if(BLINKY_G0_EXAMPLE)
    add_executable(stm32-blinky-g0 ${MAIN_SOURCE_FILE} stm32g0xx_hal_conf.h)
    target_link_libraries(stm32-blinky-g0
        HAL::STM32::G0::RCC
        HAL::STM32::G0::GPIO
        HAL::STM32::G0::CORTEX
        CMSIS::STM32::G030F6
        STM32::NoSys
    )
    stm32_print_size_of_target(stm32-blinky-g0)
endif()

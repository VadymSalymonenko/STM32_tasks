cmake_minimum_required(VERSION 3.16)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/stm32_gcc.cmake)

# Configure here which STM32 target(s) to build
option(BLINKY_G0_EXAMPLE "Compile G0 example" ON)

project(stm32-blinky C ASM)

# Collect all the .c and .h files from src and inc directories
file(GLOB SOURCES "src/*.c")
file(GLOB HEADERS "inc/*.h")

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

set(HAL_COMP_LIST RCC GPIO CORTEX)
set(CMSIS_COMP_LIST "")

if(BLINKY_G0_EXAMPLE)
    list(APPEND CMSIS_COMP_LIST STM32G0)
    list(APPEND HAL_COMP_LIST STM32G0)
endif()

find_package(CMSIS COMPONENTS "${CMSIS_COMP_LIST}" REQUIRED)
find_package(HAL COMPONENTS "${HAL_COMP_LIST}" REQUIRED)

# STM32G030F6P6-Discovery (New Executable)
if(BLINKY_G0_EXAMPLE)
    add_executable(stm32-blinky-g0 ${SOURCES} )
    
    target_include_directories(stm32-blinky-g0 PRIVATE inc)

    target_sources(stm32-blinky-g0 PRIVATE
        /opt/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_uart.c
        /opt/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_uart_ex.c
        /opt/STM32CubeG0/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal_pwr_ex.c

    )

    target_link_libraries(stm32-blinky-g0
        HAL::STM32::G0::RCC
        HAL::STM32::G0::GPIO
        HAL::STM32::G0::CORTEX
        CMSIS::STM32::G030F6
        STM32::NoSys
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

# Append the new default goal to the generated Makefile
file(APPEND ${CMAKE_BINARY_DIR}/Makefile "\n.DEFAULT_GOAL := custom_default\n")

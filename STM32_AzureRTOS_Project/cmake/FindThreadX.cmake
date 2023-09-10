# Знаходження заголовочних файлів
find_path(ThreadX_COMMON_HEADERS tx_api.h PATHS "${PROJECT_SOURCE_DIR}/../../drivers/ThreadX/common/inc")
find_path(ThreadX_PORT_HEADERS tx_port.h PATHS "${PROJECT_SOURCE_DIR}/../../drivers/ThreadX/ports/cortex_m0/gnu/inc")

# Перевірка на знайдені компоненти
if(NOT ThreadX_COMMON_HEADERS)
  message(FATAL_ERROR "Could not find common ThreadX headers. ${PROJECT_SOURCE_DIR}")
endif()

if(NOT ThreadX_PORT_HEADERS)
  message(FATAL_ERROR "Could not find port-specific ThreadX headers.")
endif()

#set(ThreadX_COMMON_SOURCES 
#    "${PROJECT_SOURCE_DIR}/../../drivers/ThreadX/common/src/*.c"
#)
#set(ThreadX_PORT_SOURCES 
#    "${PROJECT_SOURCE_DIR}/../../drivers/ThreadX/ports/cortex_m0/gnu/src/*.c"
#)
file(GLOB ThreadX_COMMON_SOURCES "${PROJECT_SOURCE_DIR}/../../drivers/ThreadX/common/src/*.c")
file(GLOB ThreadX_PORT_SOURCES 
     "${PROJECT_SOURCE_DIR}/../../drivers/ThreadX/ports/cortex_m0/gnu/src/*.c"
     "${PROJECT_SOURCE_DIR}/../../drivers/ThreadX/ports/cortex_m0/gnu/src/*.S"
     "${PROJECT_SOURCE_DIR}/../../drivers/ThreadX/ports/cortex_m0/gnu/example_build/*.S")


set(ThreadX_INCLUDE_DIRS ${ThreadX_COMMON_HEADERS} ${ThreadX_PORT_HEADERS})
set(ThreadX_FOUND TRUE)

if(ThreadX_FOUND)
  message(STATUS "Found ThreadX headers")
else ()
  message(FATAL_ERROR "Could not find ThreadX")
endif()

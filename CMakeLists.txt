cmake_minimum_required(VERSION 3.13)

include(pico-sdk/pico_sdk_init.cmake)

project(rp2040-realtime-control C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

pico_sdk_init()

add_executable(${PROJECT_NAME}
        source/main.cpp
        source/CoreOne.cpp
        source/CoreOne.h
        source/Task.cpp
        source/Task.h
        source/Scheduler.cpp
        source/Scheduler.h
        source/Globals.h
        source/pendsv_handler.S)

target_include_directories(${PROJECT_NAME} PRIVATE
        include)

target_link_libraries(${PROJECT_NAME} PRIVATE
        pico_stdlib)

pico_add_extra_outputs(${PROJECT_NAME})

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)
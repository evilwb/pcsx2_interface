cmake_minimum_required(VERSION 3.15...3.27)
project(pcsx2_interface LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
find_package(pybind11 CONFIG REQUIRED)
pybind11_add_module(pcsx2_interface src/python_wrapper.cpp src/pcsx2_interface.cpp)

add_library(pine INTERFACE)
target_include_directories(pine INTERFACE pine/)
target_link_libraries(pcsx2_interface PRIVATE pine)

if(WIN32)
    message(STATUS "Detected Windows - Adding -lws2_32 linker option")
    set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} -lws2_32")
endif()

install(TARGETS pcsx2_interface DESTINATION pcsx2_interface)

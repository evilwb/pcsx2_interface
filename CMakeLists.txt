cmake_minimum_required(VERSION 3.5...3.27)
project(pcsx2_interface LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_subdirectory(pybind11)
pybind11_add_module(pcsx2_interface python_wrapper.cpp pcsx2_interface.cpp)

add_library(pine INTERFACE)
target_include_directories(pine INTERFACE pine/)
target_link_libraries(pcsx2_interface PRIVATE pine)



cmake_minimum_required (VERSION 3.22)

project(omnik_inverter)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE DEBUG)

add_compile_options("-stdlib=libc++")
link_libraries("-stdlib=libc++")

set(HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/inverter.hpp)
set(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/inverter.cpp)

add_executable(omnik_stats main.cpp ${HEADERS} ${SOURCE})

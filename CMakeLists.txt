cmake_minimum_required(VERSION 3.5)

project(tiny-args LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(tiny-args tiny-args.hpp main.cpp)

install(TARGETS tiny-args
    LIBRARY DESTINATION
    ${CMAKE_INSTALL_LIBDIR}
)

cmake_minimum_required(VERSION 3.5)

project(ex-common LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

add_definitions(-UNDEBUG)

include_directories(
    ../../include
)

add_executable(
    ${PROJECT_NAME}
    ../../include/named-args/named-args.hpp
    main.cpp
)

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION
    ${CMAKE_INSTALL_LIBDIR}
)

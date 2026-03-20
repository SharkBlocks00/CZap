cmake_minimum_required(VERSION 3.20)
project(CZap)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
src/*.cpp
)

add_executable(
CZap ${SOURCES}
)


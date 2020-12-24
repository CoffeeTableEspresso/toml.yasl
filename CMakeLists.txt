cmake_minimum_required(VERSION 3.5.0)
project(toml)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_STANDARD 99)

include_directories(.)
link_directories(./tomlc99)

add_library(toml SHARED
        toml.c)

set_property(TARGET toml PROPERTY POSITION_INDEPENDENT_CODE ON)

target_link_libraries(toml yaslapi)
target_link_libraries(toml "${CMAKE_SOURCE_DIR}/tomlc99/libtoml.a")
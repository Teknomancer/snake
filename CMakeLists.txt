cmake_minimum_required(VERSION 2.8)

project(SNAKE)
set(CMAKE_MODULE_PATH "CMakeModules")

message("CMake source path is ${CMAKE_SOURCE_DIR}" )
message("Module path is ${CMAKE_MODULE_PATH} " )

FIND_PACKAGE(Allegro)
message("Allegro: " ${ALLEGRO_FOUND})

set(SNAKE_BUILD_DIR
    ${SNAKE_BINARY_DIR}/build
)

add_subdirectory(
    "Src"
)

include_directories(
    "Src"
)


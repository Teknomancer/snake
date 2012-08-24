cmake_minimum_required(VERSION 2.8)

project(SNAKE)

message("CMake source path is ${CMAKE_SOURCE_DIR}" )
message("Module path is ${CMAKE_MODULE_PATH} " )

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)


FIND_PACKAGE(Allegro)
message("Allegro: " ${ALLEGRO_FOUND})

set(SNAKE_BUILD_DIR
    ${SNAKE_BINARY_DIR}/bin
)

add_subdirectory(
    "Src"
)

include_directories(
    "Src"
)

link_directories(
  ${SNAKE_BINARY_DIR}
)


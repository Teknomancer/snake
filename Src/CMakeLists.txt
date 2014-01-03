set(EXECUTABLE_OUTPUT_PATH
    ${SNAKE_BUILD_DIR}
)

add_executable(snake
    "List.cpp"
    "Main.cpp"
)
#add_definitions(-std=c99)

#
# Allegro
#
include_directories(${ALLEGRO_INCLUDE_DIRS})
#link_directories(${SNAKE_BINARY_DIR})
set (LIBS ${LIBS} ${ALLEGRO_LIBRARIES})
target_link_libraries(snake ${LIBS})


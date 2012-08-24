set(EXECUTABLE_OUTPUT_PATH
    ${SNAKE_BUILD_DIR}
)

add_executable(snake
    "list.c"
    "main.c"
)
add_definitions(-std=c99)

#
# Allegro
#
include_directories(${ALLEGRO_INCLUDE_DIRS})
set (LIBS ${LIBS} ${ALLEGRO_LIBRARIES})
target_link_libraries(snake ${LIBS})


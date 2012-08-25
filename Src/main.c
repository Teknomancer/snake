/** @file
 * Snake, A highly simplistic snake game.
 * First try at using liballegro and to get a feel of it.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <allegro.h>
#include "list.h"

#define WAIT_SLICE             (60)                     /* wait time in millisec. */
#define BLOCKSIZE              (12)                     /* game grid block size */
#define SCREENWIDTH            (1024)                   /* screen width in pixels */
#define SCREENHEIGHT           (768)                    /* screen height in pixels*/
#define STATUSBAR_HEIGHT       (40)                     /* status bar height in pixels */
#define WALL_BORDER            (1)                      /* border size of a wall box */
#define MAX_SNAKE_SIZE         (400)                    /* maximum length of a snake in block size*/
#define ITEM_WALL_COLOR        makecol(140, 36, 36)
#define ITEM_SNAKE_COLOR       makecol(184, 135, 122)
#define ITEM_SNAKE_COLOR_HEAD  makecol(230, 90,  210)
#define ITEM_SNAKE_COLOR_TAIL  makecol(199, 150, 177)
#define ITEM_EMPTY_COLOR       makecol(0, 0, 0)

#define STRINGIFY(x)           #x

typedef enum map_item
{
    item_empty = 0xa,
    item_wall,
    item_snake,
    item_apple,
} map_item;

typedef enum snake_direction
{
    dir_up     = 0xf,
    dir_down,
    dir_left,
    dir_right
} snake_direction;

typedef enum collision_type
{
    collision_none = 0x4,
    collision_fatal,
    collision_food,
    collision_unknown
} collision_type;

typedef struct point
{
    int x;
    int y;
} point;

int              g_map[SCREENWIDTH / BLOCKSIZE][(SCREENHEIGHT - STATUSBAR_HEIGHT) / BLOCKSIZE];
const int        g_mapwidth  = SCREENWIDTH / BLOCKSIZE;
const int        g_mapheight = (SCREENHEIGHT - STATUSBAR_HEIGHT) / BLOCKSIZE;
snake_direction  g_snake_dir = dir_right;
LIST             g_snakebody;

void
abort_on_error(int rc, const char *message, ...)
{
    va_list fmtargs;
    char tmpbuf[2045];

    va_start(fmtargs, message);
    vsnprintf(tmpbuf, sizeof(tmpbuf) - 1, message, fmtargs);
    va_end(fmtargs);

    if (screen)
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);

    allegro_message("rc=%d %s %s", rc, tmpbuf, allegro_error);
    exit(-1);
}


void
destroy_map()
{
    while (!ListIsEmpty(&g_snakebody))
    {
        point *snake_point = ListRemoveItemAt(&g_snakebody, 0);
        free(snake_point);
    }
}


void
add_to_snake(int x, int y)
{
    point *snake_point = (point *)malloc(sizeof(point));
    if (!snake_point)
        abort_on_error(-1, "add_to_snake: insufficient memory");

    snake_point->x = x;
    snake_point->y = y;
    ListAdd(&g_snakebody, snake_point);
    g_map[x][y] = item_snake;
}


void
move_snake_to(int x, int y)
{
    point *snake_tail = ListTail(&g_snakebody);
    if (!snake_tail)
        abort_on_error(-1, "move_snake_to: missing tail");

    g_map[snake_tail->x][snake_tail->y] = item_empty;

    ListMoveTailToHead(&g_snakebody);
    point *snake_head = ListHead(&g_snakebody);
    if (!snake_head)
        abort_on_error(-1, "move_snake_to: missing head");

    snake_head->x = x;
    snake_head->y = y;
    g_map[snake_head->x][snake_head->y] = item_snake;
}


void
init_map()
{
    ListInit(&g_snakebody);

    /* borders are walls */
    for (int i = 0; i < g_mapheight; i++)
    {
        g_map[0][i] = item_wall;
        g_map[g_mapwidth - 1][i] = item_wall;
    }

    for (int k = 0; k < g_mapwidth; k++)
    {
        g_map[k][0] = item_wall;
        g_map[k][g_mapheight - 1] = item_wall;
    }

    /* set snake in middle of map */
    int snakehead_x = (g_mapwidth - 1) / 2;
    int snakehead_y = (g_mapheight - 1) / 2;
    add_to_snake(snakehead_x, snakehead_y);

    if (g_snake_dir == dir_right)
    {
        add_to_snake(snakehead_x - 1, snakehead_y);
        add_to_snake(snakehead_x - 2, snakehead_y);
    }
    else if (g_snake_dir == dir_left)
    {
        add_to_snake(snakehead_x + 1, snakehead_y);
        add_to_snake(snakehead_x + 2, snakehead_y);
    }
    else if (g_snake_dir == dir_up)
    {
        add_to_snake(snakehead_x, snakehead_y + 1);
        add_to_snake(snakehead_x, snakehead_y + 2);
    }
    else if (g_snake_dir == dir_down)
    {
        add_to_snake(snakehead_x, snakehead_y - 1);
        add_to_snake(snakehead_x, snakehead_y - 2);
    }
}


void
render_map()
{
    for (int i = 0; i < g_mapwidth; i++)
    {
        for (int k = 0; k < g_mapheight; k++)
        {
            int x = i * BLOCKSIZE;
            int y = STATUSBAR_HEIGHT + k * BLOCKSIZE;
            switch (g_map[i][k])
            {
                case item_empty:
                {
                    rectfill(screen,
                             x, y, x + BLOCKSIZE, y + BLOCKSIZE,
                             ITEM_EMPTY_COLOR);
                    break;
                }

                case item_wall:
                {
                    rectfill(screen,
                             x + WALL_BORDER, y + WALL_BORDER,
                             x + BLOCKSIZE - WALL_BORDER, y + BLOCKSIZE - WALL_BORDER,
                             ITEM_WALL_COLOR);
                    break;
                }

                case item_snake:
                {
                    int color = ITEM_SNAKE_COLOR;
                    point *head = ListHead(&g_snakebody);
                    if (i == head->x && k == head->y)
                    {
                        color = ITEM_SNAKE_COLOR;
                        //circlefill(screen,
                        //           x + BLOCKSIZE / 2, y + BLOCKSIZE / 2, BLOCKSIZE / 2 + 1,
                        //           color);
                        rectfill(screen, x, y, x  + BLOCKSIZE, y + BLOCKSIZE, color);
                    }
                    else
                    {
                        point *tail = ListTail(&g_snakebody);
                        if (i == tail->x && k == tail->y)
                        {
                            color = ITEM_SNAKE_COLOR;
                            //circlefill(screen,
                            //           x + BLOCKSIZE / 2, y + BLOCKSIZE / 2, BLOCKSIZE / 2,
                            //           color);
                            rectfill(screen, x, y, x + BLOCKSIZE, y + BLOCKSIZE, color);
                        }
                        else
                            rectfill(screen,
                                   x, y, x + BLOCKSIZE, y + BLOCKSIZE,
                                   color);
                    }

                    break;
                }
            }
        }
    }
}


int
process_input()
{
    /** @todo this needs work. simultaneous arrow keys not working properly */
    if (key[KEY_UP])
    {
        if (g_snake_dir != dir_down)
        {
            g_snake_dir = dir_up;
            return KEY_UP;
        }
    }
    else if (key[KEY_DOWN])
    {
        if (g_snake_dir != dir_up)
        {
            g_snake_dir = dir_down;
            return KEY_DOWN;
        }
    }
    else if (key[KEY_LEFT])
    {
        if (g_snake_dir != dir_right)
        {
            g_snake_dir = dir_left;
            return KEY_LEFT;
        }
    }
    else if (key[KEY_RIGHT])
    {
        if (g_snake_dir != dir_left)
        {
            g_snake_dir = dir_right;
            return KEY_RIGHT;
        }
    }
    else if (key[KEY_ESC])
    {
        return KEY_ESC;
    }

    clear_keybuf();
    return -1;
}


collision_type
detect_collision(int x, int y)
{
    switch (g_map[x][y])
    {
        case item_empty:
            return collision_none;

        case item_wall:
            printf("collided with " STRINGIFY(item_wall) "\n");
            return collision_fatal;

        case item_snake:
            printf("collided with " STRINGIFY(item_snake) "\n");
            return collision_fatal;

    }
    return collision_unknown;
}


collision_type
update_world()
{
    point *snake_head = ListItemAt(&g_snakebody, 0);
    if (!snake_head)
        abort_on_error(-1, "update_world: headless snake!");

    int snakehead_new_x  = snake_head->x;
    int snakehead_new_y  = snake_head->y;

    switch (g_snake_dir)
    {
        case dir_up:    snakehead_new_y--; break;
        case dir_down:  snakehead_new_y++; break;
        case dir_left:  snakehead_new_x--; break;
        case dir_right: snakehead_new_x++; break;
    }

    collision_type collision = detect_collision(snakehead_new_x, snakehead_new_y);
    move_snake_to(snakehead_new_x, snakehead_new_y);


    return collision;
}


void
update_screen()
{
    acquire_screen();
    render_map();
    release_screen();
}


void
clear_screen()
{
    acquire_screen();
    clear_to_color(screen, ITEM_EMPTY_COLOR);
    render_map();
    release_screen();
}


void
render_intro()
{
    int counter = 0;
    int box_width = 200;
    int box_height = 100;
    int box_x = (SCREEN_W - box_width) / 2;
    int box_y = (SCREEN_H - box_height) / 2 - 30;
    for (;;)
    {
        int color = makecol(AL_RAND() % 1, AL_RAND() % 12, AL_RAND() % 120);
        int color2 = makecol(AL_RAND() % 1, AL_RAND() % 12, AL_RAND() % 70);
        int x = AL_RAND() % box_width;
        int y = AL_RAND() % box_height;

        acquire_screen();
        rectfill(screen, box_x + x, box_y + y, box_x + x + 1, box_y + y + 1, color);
        rectfill(screen, box_x + box_width - x, box_y + box_height - y, box_x + box_width - x + 1, box_y + box_height - y + 1, color2);
        release_screen();

        if (counter > 30)
        {
            char *game_name = "S N A K E";
            char *credits = "a game by Tekn0";

            acquire_screen();
            hline(screen, box_x, box_y - 2, box_x + box_width + 1, makecol(201, 207, 0));
            textout_ex(screen, font, game_name,
                       box_x + box_width / 2 - text_length(font, game_name) / 2, box_y + box_height / 2 - text_height(font) - 10,
                       makecol(255, 255, 255), -1);
            textout_ex(screen, font, credits,
                       box_x + box_width / 2 - text_length(font, credits) / 2, box_y + box_height / 2 + text_height(font),
                       makecol(255, 255, 255), -1);
            hline(screen, box_x, box_y + box_height + 2, box_x + box_width + 1, makecol(201, 207, 0));
            release_screen();
            if (keypressed())
            {
                clear_keybuf();
                break;
            }
        }
        else
        {
            counter++;
            rest(1);
        }
    }
}

int
main(int argc, char *argv[])
{
    int rc = allegro_init();
    if (rc != 0)
    {
        printf("allegro_init failed. rc=%d\n", rc);
        return -1;
    }

    init_map();
    rc = install_timer();
    if (rc != 0)
        abort_on_error(rc, "main: Failed to install timer");

    rc = install_keyboard();
    if (rc != 0)
        abort_on_error(rc, "main: Failed to install keyboard");

    set_color_depth(32);
    rc = set_gfx_mode(GFX_AUTODETECT_WINDOWED, SCREENWIDTH, SCREENHEIGHT, 0, 0);
    if (rc != 0)
        abort_on_error(rc, "main: Unable to set gfx mode");

    set_window_title("snake");
    render_intro();
    clear_screen();
    for (;;)
    {
        collision_type collision = update_world();
        if (collision == collision_fatal)
            break;
        int pressed_key = process_input();
        if (pressed_key == KEY_ESC)
            break;
        update_screen();
        rest(WAIT_SLICE);
    }

    destroy_map();
    return 0;
}
END_OF_MAIN()


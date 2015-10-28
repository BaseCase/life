#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>

#include "board.h"


typedef uint32_t uint32;
typedef uint64_t uint64;


void handle_input(Board *world);
void draw(Board *world);
void initialize_world(Board *world);
void apply_game_rules(Board *world);
void sleep_until_frame_target(struct timeval start, struct timeval current);
void move_cursor_relative(int x, int y);
void toggle_navigate_mode();
void scroll_viewport_relative(int x, int y);


int global_running;
int global_paused;
int global_scrolling_mode;


int
main ()
{
    static struct timeval frame_start_time;
    struct timeval cur_time;

    Board *world;

    initscr();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    srand((unsigned) time(NULL));
    gettimeofday(&frame_start_time, NULL);

    world = (Board*) malloc(sizeof(Board));
    initialize_world(world);

    global_running = 1;
    global_paused = 0;
    global_scrolling_mode = 0;
    while (global_running)
    {
        handle_input(world);
        if (!global_paused)
            apply_game_rules(world);

        draw(world); // draw to a buffer but don't put it on the screen yet

        //sleep until it's time to draw buffer to screen
        gettimeofday(&cur_time, NULL);
        sleep_until_frame_target(frame_start_time, cur_time);
        refresh(); // actually put the buffer on the screen

        gettimeofday(&frame_start_time, NULL);
    }

    endwin();
    return 0;
}


void
sleep_until_frame_target (struct timeval start, struct timeval current)
{
    int ms_per_frame = 33;

    uint64 ms_start = (uint64)start.tv_sec * (uint64)1000 + (uint64)start.tv_usec / (uint64)1000;
    uint64 ms_current = (uint64)current.tv_sec * (uint64)1000 + (uint64)current.tv_usec / (uint64)1000;

    uint64 diff = ms_current - ms_start;
    assert(diff <= ms_per_frame); // for testing...make sure we're always hitting our frame target
    if (diff < ms_per_frame)
    {
        int sleep_time = (ms_per_frame - diff) * 1000;
        usleep(sleep_time);
    }
}


void
initialize_world (Board *world)
{
    int screen_width, screen_height;
    char ch;

    getmaxyx(stdscr, screen_height, screen_width);
    world->width = screen_width;
    world->height = screen_height;
    world->grid = create_grid(screen_width, screen_height);

    for (int x = 0; x < screen_width; x++)
        for (int y = 0; y < screen_height; y++)
            if ((rand() % 100) < 10)
                board_set_alive_at(world, x, y);
#if 0
    // set up 2 gliders for testing purposes...leave commented out normally
    board_set_alive_at(world, 5, 5);
    board_set_alive_at(world, 6, 6);
    board_set_alive_at(world, 4, 7);
    board_set_alive_at(world, 5, 7);
    board_set_alive_at(world, 6, 7);

    board_set_alive_at(world, 30, 30);
    board_set_alive_at(world, 29, 29);
    board_set_alive_at(world, 29, 28);
    board_set_alive_at(world, 30, 28);
    board_set_alive_at(world, 31, 28);
#endif
}


void
handle_input (Board *world)
{
    int input;

    input = getch();
    switch (input)
    {
        case 'q':
            global_running = 0;
            break;

        case 'p':
            global_paused = !global_paused;
            break;

        case ' ':
            board_toggle_cell_at_cursor(world);
            break;

        case 'n':
            toggle_navigate_mode();
            break;

        case KEY_UP:
        case 'k':
            if (global_scrolling_mode)
                scroll_viewport_relative(0, -1);
            else
                move_cursor_relative(0, -1);
            break;

        case 'K':
            if (global_scrolling_mode)
                scroll_viewport_relative(0, -10);
            else
                move_cursor_relative(0, -10);
            break;

        case KEY_DOWN:
        case 'j':
            if (global_scrolling_mode)
                scroll_viewport_relative(0, +1);
            else
                move_cursor_relative(0, +1);
            break;

        case 'J':
            if (global_scrolling_mode)
                scroll_viewport_relative(0, +10);
            else
                move_cursor_relative(0, +10);
            break;

        case KEY_LEFT:
        case 'h':
            if (global_scrolling_mode)
                scroll_viewport_relative(-1, 0);
            else
                move_cursor_relative(-1, 0);
            break;

        case 'H':
            if (global_scrolling_mode)
                scroll_viewport_relative(-10, 0);
            else
                move_cursor_relative(-10, 0);
            break;

        case KEY_RIGHT:
        case 'l':
            if (global_scrolling_mode)
                scroll_viewport_relative(+1, 0);
            else
                move_cursor_relative(+1, 0);
            break;

        case 'L':
            if (global_scrolling_mode)
                scroll_viewport_relative(+10, 0);
            else
                move_cursor_relative(+10, 0);
            break;
    }
}


void
draw (Board *world)
{
    int x, y, init_x, init_y, max_x, max_y;
    char ch;

    getyx(stdscr, init_y, init_x);
    getmaxyx(stdscr, max_y, max_x);

    for (x=0; x < world->width; x++)
        for (y=0; y < world->height; y++)
        {
            ch = board_is_alive_at(world, x, y)? '#' : ' ';
            mvaddch(y, x, ch);
        }


    if (global_scrolling_mode)
    {
        move(3, 4);
        hline('-', max_x-8);
        move(max_y-4, 4);
        hline('-', max_x-8);
        move(3, 5);
        vline('|', max_y-6);
        move(3, max_x-6);
        vline('|', max_y-6);
    }

    move(init_y, init_x);
}


void
apply_game_rules (Board *board)
{
    int x, y, neighbor_count;
    char alive;
    int tick_ms = 500;
    uint64 cur_ms, last_tick_ms;
    struct timeval cur_time;
    static struct timeval last_tick_time = {};
    static Board buffer_board = {};

    if (last_tick_time.tv_sec == 0)
        gettimeofday(&last_tick_time, NULL);

    if (buffer_board.width == 0)
    {
        buffer_board.width = board->width;
        buffer_board.height = board->height;
        buffer_board.grid = create_grid(buffer_board.width, buffer_board.height);
    }

    gettimeofday(&cur_time, NULL);
    last_tick_ms = (uint64)last_tick_time.tv_sec * (uint64)1000 + (uint64)last_tick_time.tv_usec / (uint64)1000;
    cur_ms = (uint64)cur_time.tv_sec * (uint64)1000 + (uint64)cur_time.tv_usec / (uint64)1000;

    if (cur_ms - last_tick_ms >= tick_ms)
    {
        for (y = 0; y < board->height; y++)
            for (x = 0; x < board->width; x++)
            {
                neighbor_count = board_count_living_neighbors_at_position(board, x, y);
                switch (neighbor_count)
                {
                    case 2:
                        alive = board_is_alive_at(board, x, y);
                        break;

                    case 3:
                        alive = 1;
                        break;

                    case 0:
                    case 1:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                    case 8:
                    default:
                        alive = 0;
                        break;
                }
                if (alive)
                    board_set_alive_at(&buffer_board, x, y);
                else
                    board_set_dead_at(&buffer_board, x, y);
            }
        char *tmp = board->grid;
        board->grid = buffer_board.grid;
        buffer_board.grid = tmp;

        last_tick_time = cur_time;
    }
}


void
move_cursor_relative (int x, int y)
{
    int cur_x, cur_y;
    getyx(stdscr, cur_y, cur_x);
    move(cur_y + y, cur_x + x);
}


void
toggle_navigate_mode ()
{
    global_scrolling_mode = !global_scrolling_mode;
}


void scroll_viewport_relative (int x, int y)
{
    // not 100% how I want to do this yet...gotta think about it.
    // the idea is that the box will move around and only display part of the full universe.
    // this feature necessitates some rendering code rework.
}

#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include <unistd.h>


typedef uint32_t uint32;
typedef uint64_t uint64;


typedef struct Board
{
    int width;
    int height;
    char **grid;  // best to only access this via helper functions
} Board;


void handle_input(Board *world);
void draw(Board *world);
void initialize_world(Board *world);
void apply_game_rules(Board *world);
void sleep_until_frame_target(struct timeval start, struct timeval current);

// board-specific functions
void board_initialize_grid(Board *board, int width, int height);
int board_is_alive_at(Board *board, int x, int y);
void board_set_alive_at(Board *board, int x, int y);
void board_set_dead_at(Board *board, int x, int y);


int global_running;


int
main ()
{
    static struct timeval frame_start_time;
    struct timeval cur_time;

    Board *world;

    initscr();
    noecho();
    nodelay(stdscr, TRUE);
    srand((unsigned) time(NULL));
    gettimeofday(&frame_start_time, NULL);

    world = (Board*) malloc(sizeof(Board));
    initialize_world(world);

    global_running = 1;
    while (global_running)
    {
        handle_input(world);
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
    int ms_per_frame = 50;

    uint64 ms_start = (uint64)start.tv_sec * (uint64)1000 + (uint64)start.tv_usec / (uint64)1000;
    uint64 ms_current = (uint64)current.tv_sec * (uint64)1000 + (uint64)current.tv_usec / (uint64)1000;

    uint64 diff = ms_current - ms_start;
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
    board_initialize_grid(world, screen_width, screen_height);

    for (int x = 0; x < screen_width; x++)
        for (int y = 0; y < screen_height; y++)
            if ((rand() % 100) < 5)
                board_set_alive_at(world, x, y);
}


void
handle_input (Board *world)
{
    char input;

    input = getch();
    switch (input)
    {
        case 'a':
            board_set_alive_at(world, 3, 5);
            break;

        case 'd':
            board_set_dead_at(world, 3, 5);
            break;

        case 'q':
            global_running = 0;
            break;
    }
}


void
draw (Board *world)
{
    int x, y;
    char ch;
    for (x=0; x < world->width; x++)
        for (y=0; y < world->height; y++)
        {
            ch = board_is_alive_at(world, x, y)? '#' : ' ';
            mvaddch(y, x, ch);
        }
}


void
board_initialize_grid (Board *board, int width, int height)
{
    int x, y;
    char alive;

    board->grid = (char **) malloc(sizeof(char *) * height);

    for (y = 0; y < height; y++)
    {
        board->grid[y] = (char *) malloc(sizeof(char) * width);
        for (x = 0; x < width; x++)
            board->grid[y][x] = 0;
    }
}


int
board_is_alive_at (Board *board, int x, int y)
{
    return board->grid[y][x] == 1;
}


void
board_set_alive_at (Board *board, int x, int y)
{
    board->grid[y][x] = 1;
}


void
board_set_dead_at (Board *board, int x, int y)
{
    board->grid[y][x] = 0;
}


void
apply_game_rules (Board *board)
{
    int x, y;
    int tick_ms = 500;
    uint64 cur_ms, last_tick_ms;
    struct timeval cur_time;
    static struct timeval last_tick_time = {};
    if (last_tick_time.tv_sec == 0)
        gettimeofday(&last_tick_time, NULL);

    gettimeofday(&cur_time, NULL);
    last_tick_ms = (uint64)last_tick_time.tv_sec * (uint64)1000 + (uint64)last_tick_time.tv_usec / (uint64)1000;
    cur_ms = (uint64)cur_time.tv_sec * (uint64)1000 + (uint64)cur_time.tv_usec / (uint64)1000;

    if (cur_ms - last_tick_ms >= tick_ms)
    {
        for (y = 0; y < board->height; y++)
            for (x = 0; x < board->width; x++)
                board->grid[y][x] = !board->grid[y][x];
        last_tick_time = cur_time;
    }
}

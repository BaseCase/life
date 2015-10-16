#include <curses.h>
#include <stdlib.h>
#include <time.h>


typedef struct Board
{
    int width;
    int height;
    char **grid;  // best to only access this via helper functions
} Board;


void handle_input(Board *world);
void animate(Board *world);
void initialize_world(Board *world);

// board-specific functions
void board_initialize_grid(Board *board, int width, int height);
int board_is_alive_at(Board *board, int x, int y);
void board_set_alive_at(Board *board, int x, int y);
void board_set_dead_at(Board *board, int x, int y);


int global_running;


int
main ()
{
    Board *world;

    initscr();
    noecho();
    nodelay(stdscr, TRUE);
    srand((unsigned) time(NULL));

    world = (Board*) malloc(sizeof(Board));
    initialize_world(world);

    global_running = 1;
    while (global_running)
    {
        handle_input(world);
        animate(world);
    }

    endwin();
    return 0;
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
animate (Board *world)
{
    int x, y;
    char ch;

    for (x = 0; x < world->width; x++)
        for (y = 0; y < world->height; y++)
        {
            if (board_is_alive_at(world, x, y))
                ch = '#';
            else
                ch = ' ';
            mvaddch(y, x, ch);
        }
    refresh();
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


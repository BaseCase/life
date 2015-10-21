#include <curses.h>

#include "board.h"

char*
create_grid (int width, int height)
{
    int i;
    char *grid;

    grid = (char *) malloc(sizeof(char) * height * width);

    for (i=0; i < height * width; i++)
        grid[i] = 0;

    return grid;
}


int
board_is_alive_at (Board *board, int x, int y)
{
    int pos = (y * board->width) + x;
    return board->grid[pos] == 1;
}


void
board_set_alive_at (Board *board, int x, int y)
{
    int pos = (y * board->width) + x;
    board->grid[pos] = 1;
}


void
board_set_dead_at (Board *board, int x, int y)
{
    int pos = (y * board->width) + x;
    board->grid[pos] = 0;
}


int
board_count_living_neighbors_at_position (Board *board, int x, int y)
{
    int count = 0;

    // upper left
    if ((x > 0) &&
        (y > 0) &&
        board_is_alive_at(board, x-1, y-1))
        ++count;

    // above
    if ((y > 0) &&
        board_is_alive_at(board, x, y-1))
        ++count;

    // upper right
    if ((y > 0) &&
        (x < board->width-1) &&
        board_is_alive_at(board, x+1, y-1))
        ++count;

    // left
    if ((x > 0) &&
        board_is_alive_at(board, x-1, y))
        ++count;

    // right
    if ((x < board->width-1) &&
        board_is_alive_at(board, x+1, y))
        ++count;

    // lower left
    if ((x > 0) &&
        (y < board->height-1) &&
        board_is_alive_at(board, x-1, y+1))
        ++count;

    // below
    if ((y < board->height-1) &&
        board_is_alive_at(board, x, y+1))
        ++count;

    // lower right
    if ((y < board->height-1) &&
        (x < board->width-1) &&
        board_is_alive_at(board, x+1, y+1))
        ++count;

    return count;
}


void
board_toggle_cell_at_cursor (Board *board)
{
    int x, y;
    char alive;
    getyx(stdscr, y, x);
    alive = board_is_alive_at(board, x, y);
    if (alive)
        board_set_dead_at(board, x, y);
    else
        board_set_alive_at(board, x, y);
}

#ifndef BOARD_H

typedef struct Board
{
    int width;
    int height;
    char *grid;
} Board;


// board-specific functions
char* create_grid(int width, int height);
int board_is_alive_at(Board *board, int x, int y);
void board_set_alive_at(Board *board, int x, int y);
void board_set_dead_at(Board *board, int x, int y);
int board_count_living_neighbors_at_position(Board *board, int x, int y);
void board_toggle_cell_at_cursor(Board *board);


#define BOARD_H
#endif

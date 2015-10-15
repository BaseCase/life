#include <curses.h>
#include <stdlib.h>
#include <time.h>


void initialize_board();


int
main ()
{
    int running;
    char input;

    initscr();
    noecho();
    srand((unsigned) time(NULL));

    initialize_board();

    running = 1;
    while (running)
    {
        input = getch();

        switch (input)
        {
            case 'q':
                running = 0;
                break;
        }
    }

    endwin();
    return 0;
}


void
initialize_board ()
{
    int screen_width, screen_height;
    char ch;

    // randomly set starting position, each cell has a 5% chance of being alive
    getmaxyx(stdscr, screen_height, screen_width);
    for (int x = 0; x < screen_width; x++)
        for (int y = 0; y < screen_height; y++)
        {
            if (rand() % 100 < 5)
                ch = '#';
            else
                ch = ' ';
            mvaddch(y, x, ch);
        }
}

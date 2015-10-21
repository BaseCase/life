#!/bin/bash


set -e

touch bin/life && rm bin/life
gcc -g -lncurses -o bin/life src/main.c src/board.c
bin/life

#!/bin/bash


set -e

touch bin/life && rm bin/life
gcc -lncurses -o bin/life src/*
bin/life

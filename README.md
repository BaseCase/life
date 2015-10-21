# Conway's Game of Life

## TODO - for v1

- [ ] when paused, you can edit the board by moving cursor around and toggling cells
- [ ] when screen shrinks, maintain offscreen board but don't try to render
- [ ] when screen grows, increase board size to match
- [ ] unit tests (figure out how to do this in C)
- [ ] figure out configure/make/make install thing
- [ ] create homebrew formula
- [ ] submit to homebrew/games
- [ ] refactor: move board struct and functions into separate file/header
- [ ] refactor: storing too many timeval structs...could probably pass some around as params and only snap them once
- [ ] refactor: too many casts...actually understand what you're doing and you won't need so many `(uint64)`s all over
- [ ] refactor: clean up neighbor counting...lots of duplicate checks right now
- [x] spacebar pauses/unpauses game
- [x] BUGFIX: something is not right...the forms being created don't look correct...
- [x] apply game of life rules each frame
- [x] animate
- [x] fix animation clock to use wall clock and not CPU clock (which doesn't work well at all)
- [x] match playing area to window size
- [x] randomly generate starting board on launch (maybe each cell has a 5% chance of being alive? something like that?)


## TODO - for later on

* allow scrolling viewport when board is larger than screen
* read starting board from a file
* save current position in a file
* mouse support for edit mode
* +/- speed up/slow down animation
* improve game loop timer so it doesn't burn up lots of CPU it's not using

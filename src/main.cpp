#include <ncurses.h>
#include "tetris.hpp"

int main() {
	// initialization
    initscr();              // start curses mode
    if (!has_colors()) {
		endwin();
        return 0;
    }

	Tetris tetris = Tetris();
	tetris.run();

	endwin();
    return 0;
}

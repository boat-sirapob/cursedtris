#include "tetris.hpp"
#include <chrono>
#include <ncurses.h>
#include <random>

bool Piece::is_colliding() {
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            const int board_x = this->x + x;
            const int board_y = this->y + y;
            if (at(x, y) != 0 && (board->at(board_x, board_y) || !board->in_range(board_x, board_y))) {
                return true;
            }
        }
    }
    return false;
}

void Piece::rotate_left() {
    rotation = rotation == 0 ? 3 : rotation - 1;
}

void Piece::rotate_right() {
    rotation = (rotation + 1) % 4;
}

void Piece::center_vertically() {
    x = board->get_width() / 2 - 2;
}

void Player::init_pieces() {
    current_piece = Piece(board, pieces_distr(eng));
    next_piece = Piece(board, pieces_distr(eng));

    current_piece.center_vertically();
}

NCURSES_COLOR_T Player::at(int x, int y) {
    const int virtual_x = x - current_piece.x;
    const int virtual_y = y - current_piece.y;
    
    return current_piece.at(virtual_x, virtual_y);
}

void Player::hard_drop() {
    // todo
}

void Player::soft_drop() {
    // check if at bottom
    if (is_colliding(Down)) {
        place();
        return;
    }

    fall_timer = current_fall_time;
    current_piece.y += 1;
}

void Player::move_left() {
    if (!is_colliding(Left)) {
        current_piece.x -= 1;
    }
}

void Player::move_right() {
    if (!is_colliding(Right)) {
        current_piece.x += 1;
    }
}

void Player::rotate_left() {
    Piece new_piece(current_piece);
    new_piece.rotate_left();

    if (!new_piece.is_colliding()) {
        current_piece.rotate_left();
    }
}

void Player::rotate_right() {
    Piece new_piece(current_piece);
    new_piece.rotate_right();

    if (!new_piece.is_colliding()) {
        current_piece.rotate_right();
    }
}

void Player::update() {
    const auto end_time = std::chrono::high_resolution_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    start_time = end_time;

    fall_timer -= elapsed.count();

    if (fall_timer <= 0) {
        fall_timer = current_fall_time;
        fall();
    }
}

bool Player::is_colliding(Direction direction) {
    Piece new_piece(current_piece);
    DirectionVector* direction_vector = &dir_map[direction];
    new_piece.x += direction_vector->x;
    new_piece.y += direction_vector->y;

    return new_piece.is_colliding();
}

void Player::fall() {
    // handle when fall timer times out

    // check if at bottom
    if (is_colliding(Down)) {
        place();
        return;
    }
    current_piece.y += 1;
}

void Player::place() {
    board->place();
    board->clear_lines();
}

void Board::place() {
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            const NCURSES_COLOR_T player_state = player.current_piece.at(x, y);
            if (player_state) {
                state[player.current_piece.y + y][player.current_piece.x + x] = player_state;
            }
        }
    }

    // generate next random
    player.current_piece = player.next_piece;
    player.next_piece = Piece(this, pieces_distr(eng));
    player.current_piece.center_vertically();

    if (player.current_piece.is_colliding()) {
        tetris->lose();
    }
}

void Board::clear_lines() {
    // Board new_board(*this); // ? use temp board maybe to show lines to clear before actually clearing?

    for (int line = 0; line < height; line++) {
        bool to_clear = true;
        for (int col = 0; col < width; col++) {
            if (at(col, line) == 0) {
                to_clear = false;
                break;
            }
        }
        if (to_clear) {
            state.erase(state.begin()+line);
            state.insert(state.begin(), vector<NCURSES_COLOR_T>(width, 0));
        }
    }
}

Tetris::Tetris(): board(this), board_window(board.get_width() * 2 + 2, board.get_height() + 2) {
    // raw();
    cbreak();               // disable line buffering
    keypad(stdscr, TRUE);   // for arrow keys and function keys
    noecho();               // prevent echoing user chars
    curs_set(0);			// hide cursor
    nodelay(stdscr, true);	// non blocking getch
    timeout(50);	// timeout getch

    // init colors
    start_color();
    use_default_colors();
    init_color(COLOR_ORANGE, 800, 300, 0);
    init_pair(COLOR_BLACK, COLOR_BLACK, COLOR_TRANSPARENT);
    init_pair(COLOR_RED, COLOR_BLACK, COLOR_RED);
    init_pair(COLOR_GREEN, COLOR_BLACK, COLOR_GREEN);
    init_pair(COLOR_YELLOW, COLOR_BLACK, COLOR_YELLOW);
    init_pair(COLOR_BLUE, COLOR_BLACK, COLOR_BLUE);
    init_pair(COLOR_MAGENTA, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(COLOR_CYAN, COLOR_BLACK, COLOR_CYAN);
    init_pair(COLOR_WHITE, COLOR_BLACK, COLOR_WHITE);
    init_pair(COLOR_ORANGE, COLOR_BLACK, COLOR_ORANGE);

    // wborder(board_window.window, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    board_window.center_window();
    refresh(); // ? without this line, board is not shown at the start until button is pressed
}

Tetris::~Tetris() {
    // endwin();
}

void Tetris::update() {
    board.player.update();
}

void Tetris::display() {
    // mvprintw(0, 0, "player.x: %d", board.player.current_piece.x);
    // mvprintw(1, 0, "player.y: %d", board.player.current_piece.y);

    // mvprintw(4, 0, "board.get_height(): %d", board.get_height());
    // mvprintw(5, 0, "board.get_width(): %d", board.get_width());

    box(board_window.window, 0, 0);

    for (int row = 0; row < board.get_height(); row++) {
        for (int col = 0; col < board.get_width(); col++) {
            const NCURSES_COLOR_T placed_cell = board.at(col, row);
            const NCURSES_COLOR_T virtual_cell = board.player.at(col, row);

            if (placed_cell) {
                wattron(board_window.window, COLOR_PAIR(placed_cell));
            } else {
                wattron(board_window.window, COLOR_PAIR(virtual_cell));
            }

            mvwprintw(board_window.window, row + 1, col*2 + 1, "  ");
            wattroff(board_window.window, COLOR_PAIR(placed_cell));
            wattroff(board_window.window, COLOR_PAIR(virtual_cell));
        }
    }
    wrefresh(board_window.window);
}

void Tetris::handle_input(int chr) {
    switch (chr) {
        case 'z': {
            board.player.rotate_left();
            break;
        }
        case 'x': {
            board.player.rotate_right();
            break;
        }
        case KEY_DOWN: {
            board.player.soft_drop();
            break;
        }
        case KEY_LEFT: {
            board.player.move_left();
            break;
        }
        case KEY_RIGHT: {
            board.player.move_right();
            break;
        }
        case KEY_RESIZE: {
            board_window.center_window();
            break;
        }
        case KEY_EXIT: {
            running = false;
            break;
        }
        default: {
            break;
        }
    }
}

void Tetris::run() {
    running = true;
    int chr;
    while (running) {
        // display game
        update();
        display();

        chr = wgetch(stdscr);
        
        // handle input
        handle_input(chr);

        refresh();
    }
}

void Tetris::lose() {
    // todo: reset
    running = false;
}
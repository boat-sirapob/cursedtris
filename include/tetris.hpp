#ifndef TETRIS_HPP
#define TETRIS_HPP

#include "window.hpp"
#include <cstdint>
#include <chrono>
#include <random>
#include <unordered_map>
#include <vector>
#include <ncurses.h>

#define TETRIS_STANDARD_GRID_WIDTH 10
#define TETRIS_STANDARD_GRID_HEIGHT 20

#define COLOR_TRANSPARENT -1
#define COLOR_ORANGE 8

using std::vector;

enum Direction {
    Left,
    Right,
    Up,
    Down
};

struct DirectionVector {
    int x;
    int y;
};

static std::unordered_map<Direction, DirectionVector> dir_map = {
    {Left, {-1, 0}},
    {Right, {1, 0}},
    {Up, {0, -1}},
    {Down, {0, 1}}
};

struct PieceType {
    NCURSES_COLOR_T color;
    bool rotations[4][4][4];
};

static std::vector<PieceType> pieces = {
    { // I piece
        COLOR_CYAN,
        {
            {
                {0,0,0,0},
                {0,0,0,0},
                {1,1,1,1},
                {0,0,0,0},
            },
            {
                {0,0,1,0},
                {0,0,1,0},
                {0,0,1,0},
                {0,0,1,0},
            },
            {
                {0,0,0,0},
                {0,0,0,0},
                {1,1,1,1},
                {0,0,0,0},
            },
            {
                {0,0,1,0},
                {0,0,1,0},
                {0,0,1,0},
                {0,0,1,0},
            }
        }
    },
    { // J piece
        COLOR_BLUE,
        {
            {
                {0,0,0,0},
                {1,1,1,0},
                {0,0,1,0},
                {0,0,0,0},
            },
            {
                {0,1,0,0},
                {0,1,0,0},
                {1,1,0,0},
                {0,0,0,0},
            },
            {
                {1,0,0,0},
                {1,1,1,0},
                {0,0,0,0},
                {0,0,0,0},
            },
            {
                {0,1,1,0},
                {0,1,0,0},
                {0,1,0,0},
                {0,0,0,0},
            }
        }
    },
    { // L piece
        COLOR_ORANGE,
        {
            {
                {0,0,0,0},
                {1,1,1,0},
                {1,0,0,0},
                {0,0,0,0},
            },
            {
                {1,1,0,0},
                {0,1,0,0},
                {0,1,0,0},
                {0,0,0,0},
            },
            {
                {0,0,1,0},
                {1,1,1,0},
                {0,0,0,0},
                {0,0,0,0},
            },
            {
                {0,1,0,0},
                {0,1,0,0},
                {0,1,1,0},
                {0,0,0,0},
            }
        }
    },
    { // O piece
        COLOR_YELLOW,
        {
            {
                {0,0,0,0},
                {0,1,1,0},
                {0,1,1,0},
                {0,0,0,0},
            },
            {
                {0,0,0,0},
                {0,1,1,0},
                {0,1,1,0},
                {0,0,0,0},
            },
            {
                {0,0,0,0},
                {0,1,1,0},
                {0,1,1,0},
                {0,0,0,0},
            },
            {
                {0,0,0,0},
                {0,1,1,0},
                {0,1,1,0},
                {0,0,0,0},
            }
        }
    },
    { // S piece
        COLOR_GREEN,
        {
            {
                {0,0,0,0},
                {0,1,1,0},
                {1,1,0,0},
                {0,0,0,0},
            },
            {
                {0,1,0,0},
                {0,1,1,0},
                {0,0,1,0},
                {0,0,0,0},
            },
            {
                {0,0,0,0},
                {0,1,1,0},
                {1,1,0,0},
                {0,0,0,0},
            },
            {
                {0,1,0,0},
                {0,1,1,0},
                {0,0,1,0},
                {0,0,0,0},
            }
        }
    },
    { // T piece
        COLOR_MAGENTA,
        {
            {
                {0,0,0,0},
                {1,1,1,0},
                {0,1,0,0},
                {0,0,0,0},
            },
            {
                {0,1,0,0},
                {1,1,0,0},
                {0,1,0,0},
                {0,0,0,0},
            },
            {
                {0,1,0,0},
                {1,1,1,0},
                {0,0,0,0},
                {0,0,0,0},
            },
            {
                {0,1,0,0},
                {0,1,1,0},
                {0,1,0,0},
                {0,0,0,0},
            }
        }
    },
    { // Z piece
        COLOR_RED,
        {
            {
                {0,0,0,0},
                {1,1,0,0},
                {0,1,1,0},
                {0,0,0,0},
            },
            {
                {0,0,1,0},
                {0,1,1,0},
                {0,1,0,0},
                {0,0,0,0},
            },
            {
                {0,0,0,0},
                {1,1,0,0},
                {0,1,1,0},
                {0,0,0,0},
            },
            {
                {0,0,1,0},
                {0,1,1,0},
                {0,1,0,0},
                {0,0,0,0},
            }
        }
    }
};

class Board;
class Tetris;

class Piece {
public:
    Piece(): board(nullptr), x(0), y(0), rotation(0), type(nullptr) {}
    Piece(Board* b, int index): board(b), x(0), y(0), rotation(0), type(&pieces[index]) {}

    NCURSES_COLOR_T at(int x, int y) {
        if (0 <= x && x < 4 && 0 <= y && y < 4) {
            return type->rotations[rotation][y][x] * type->color;
        } else {
            return 0;
        }
    }

    bool is_colliding();
    void rotate_left();
    void rotate_right();
    void center_vertically();

    Board* board;
    PieceType* type;
    int x;
    int y;
private:
    uint8_t rotation;
};

// random
static std::random_device rd;
static std::default_random_engine eng(rd());
static auto pieces_distr = std::uniform_int_distribution<>(0, pieces.size() - 1);

class Player {
public:
    Player(Board* b): board(b) {
        start_time = std::chrono::high_resolution_clock::now();
        init_pieces();
    }

    void hard_drop();
    void soft_drop();
    void move_left();
    void move_right();
    void rotate_left();
    void rotate_right();

    void update();
    void init_pieces();
    NCURSES_COLOR_T at(int x, int y);

    static constexpr double INITIAL_FALL_TIME = 200.0; // ms
    double current_fall_time = INITIAL_FALL_TIME;
    double fall_timer = current_fall_time;
    
    Piece current_piece;
    Piece next_piece;
private:
    bool will_collide_rotate(int rotate);
    bool is_colliding(Direction direction);

    void fall();
    void place();

    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    Board* board;
};

class Board {
private:
    Tetris* tetris;
    int width;
    int height;
    vector<vector<NCURSES_COLOR_T>> state;
public:
    Board(Tetris* t): tetris(t), player(this), width(TETRIS_STANDARD_GRID_WIDTH), height(TETRIS_STANDARD_GRID_HEIGHT), state(height, vector<NCURSES_COLOR_T>(width, 0)) {}
    Board(Tetris* t, int w, int h): tetris(t), player(this), width(w), height(h), state(height, vector<NCURSES_COLOR_T>(width, 0)) {}
    
    int get_width() {
        return width;
    }
    int get_height() {
        return height;
    }
    NCURSES_COLOR_T at(int x, int y) {
        return in_range(x, y) ? state[y][x] : 0;
    }
    bool in_range(int x, int y) {
        return (0 <= x && x < width) && (0 <= y && y < height);
    }

    void place();
    void clear_lines();
    Player player;
};

class Tetris {
public:
    Tetris();
    ~Tetris();

    void update();
    void display();
    void handle_input(int chr);
    void run();
    void lose();

private:
    Board board;
    Window board_window;
    bool running = false;
};

#endif // TETRIS_HPP
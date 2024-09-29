#include "window.hpp"
#include <ncurses.h>

Window::Window(int w, int h): width(w), height(h) {
    window = newwin(height, width, 0, 0);
    keypad(window, TRUE);
}

Window::~Window() {
    delwin(window);
}

void Window::center_window() {
    int screen_height;
    int screen_width;
    getmaxyx(stdscr, screen_height, screen_width);

    // todo: handle case when screen geometry less than window geometry
    mvwin(window, (screen_height - height) / 2, (screen_width - width) / 2);
}

void Window::hcenter_window(int y) {
    int screen_width = getmaxx(stdscr);

    mvwin(window, y, (screen_width - width) / 2);
}

void Window::vcenter_window(int x) {
    int screen_height = getmaxy(stdscr);

    mvwin(window, (screen_height - height) / 2, x);
}
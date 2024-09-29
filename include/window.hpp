#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <ncurses.h>

class Window {
public:
    Window(int w, int h);
    ~Window();
    
    void center_window();
    void hcenter_window(int y);
    void vcenter_window(int x);

    WINDOW* window;
    int width;
    int height;
private:
};

#endif // WINDOW_HPP
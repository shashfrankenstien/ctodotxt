#pragma once

#include "todo.h"


struct curpos {
    int col;
    int line;
};

typedef struct {
    char* title;
    TodoArray* todos;
    struct curpos cpos; // command cursor position
    struct curpos vcpos; // virtual cursor position (0 indexed)
    struct curpos minpos; // window size
    struct curpos maxpos; // window size
    int scroll_state;
} TodoUI;



TodoUI todoui_init(TodoArray* t, char* title);
int todoui_draw(TodoUI* u);
int todoui_mv_up(TodoUI* u, int n);
int todoui_mv_down(TodoUI* u, int n);

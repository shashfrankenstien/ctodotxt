#pragma once

#include "todo.h"


struct curpos {
    int col;
    int line;
};

typedef struct {
    TodoArray* todos;
    struct curpos cpos; // real cursor position
    struct curpos vcpos; // virtual cursor position
    struct curpos maxpos; // window size
    int selected_idx;
} TodoUI;



TodoUI todoui_init(TodoArray* t);
int todoui_draw(TodoUI* u);
int todoui_mv_up(TodoUI* u, int n);
int todoui_mv_down(TodoUI* u, int n);

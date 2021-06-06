#include <stdio.h>

#include "todo.h"
#include "todoui.h"
#include "ansictrl.h"


TodoUI todoui_init(TodoArray* t)
{
    int lines, cols;
    get_console_size(&lines, &cols);
    // printf("ANSII codes: %s\n", (supports_ansii()) ? "TRUE" : "FALSE");
    // printf("Size: %d x %d\n", cols, lines);

    return (TodoUI){
        .todos=t,
        .cpos=(struct curpos){0,20},
        .vcpos=(struct curpos){0,3},
        .maxpos=(struct curpos){cols,lines},
        .selected_idx = 0
    };
}


static void print_todo(TodoUI* u, int todo_idx)
{
    console_clear_line();
    Todo* t = &u->todos->todos[todo_idx];
    if (u->selected_idx == todo_idx) {
        printf("%s%s%d - %s%s\n", COLOR_BOLD, COLOR_INVERSE, t->tid, t->raw_todo, COLOR_RESET);
    }
    else {
        printf("%d - %s\n", t->tid, t->raw_todo);
    }
}

static void draw_line(TodoUI* u, int on_line)
{
    cursor_position(on_line, u->cpos.col);
    char c = '-';
    printf(COLOR_YELLOW);
    for (int i = 0; i < u->maxpos.col; i ++) {
        putchar(c);
    }
    printf(COLOR_RESET);
}


int todoui_draw(TodoUI* u)
{
    console_clear_screen();
    draw_line(u, u->vcpos.line-1);
    cursor_position(u->vcpos.line, u->vcpos.col);

    for (int i=0; i<u->todos->n_todos; i++) {
        print_todo(u, i);
    }

    draw_line(u, u->cpos.line-1);
    cursor_position(u->cpos.line, u->cpos.col);
    return 0;
}


int todoui_mv_up(TodoUI* u, int n)
{
    if (u->selected_idx >= n) {
        cursor_position(u->vcpos.line, u->cpos.col);
        u->selected_idx -= n;
        u->vcpos.line -= n;
        print_todo(u, u->selected_idx+n);
        cursor_position(u->vcpos.line, u->cpos.col);
        print_todo(u, u->selected_idx);
    }

    cursor_position(u->cpos.line, u->cpos.col);
    return 0;
}

int todoui_mv_down(TodoUI* u, int n)
{
    if (u->selected_idx < u->todos->n_todos-n) {
        cursor_position(u->vcpos.line, u->cpos.col);
        u->selected_idx += n;
        u->vcpos.line += n;
        print_todo(u, u->selected_idx-n);
        cursor_position(u->vcpos.line, u->cpos.col);
        print_todo(u, u->selected_idx);

    }

    cursor_position(u->cpos.line, u->cpos.col);
    return 0;
}

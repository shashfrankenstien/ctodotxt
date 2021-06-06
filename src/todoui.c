#include <stdio.h>
#include <string.h>

#include "todo.h"
#include "todoui.h"
#include "ansictrl.h"


#define TOP_OFFSET 4 // includes 3 lines for the title and 1 line for border
#define PAGE_SIZE 15 // number of todos to display per page

TodoUI todoui_init(TodoArray* t, char* title)
{
    int lines, cols;
    get_console_size(&lines, &cols);
    // printf("ANSII codes: %s\n", (supports_ansii()) ? "TRUE" : "FALSE");
    // printf("Size: %d x %d\n", cols, lines);

    return (TodoUI){
        .title = title,
        .todos=t,
        .cpos=(struct curpos){1,TOP_OFFSET+PAGE_SIZE+1},
        .vcpos=(struct curpos){0,0},
        .minpos=(struct curpos){1,1},
        .maxpos=(struct curpos){cols,lines},
        .scroll_state = 0
    };
}


static void print_todo(TodoUI* u, int todo_idx)
{
    console_clear_line();
    Todo* t = &u->todos->todos[todo_idx];
    if (u->vcpos.line + u->scroll_state == todo_idx) {
        printf("%s%s%d - %s%s\n", COLOR_BOLD, COLOR_INVERSE, t->tid, t->raw_todo, COLOR_RESET);
    }
    else {
        printf("%d - %s\n", t->tid, t->raw_todo);
    }
}

static void draw_line(TodoUI* u, int on_line)
{
    cursor_position(on_line, u->minpos.col);
    char c = '-';
    printf(COLOR_YELLOW);
    for (int i = u->minpos.col; i < u->maxpos.col; i ++) {
        putchar(c);
    }
    printf(COLOR_RESET);
}

static void print_title(TodoUI* u)
{
    // int col = (u->maxpos.col - u->minpos.col - strlen(u->title)) / 2; // centered
    int col = 2;
    cursor_position(u->minpos.line+1, col);
    printf(COLOR_YELLOW);
    printf("%s", u->title);
    printf(COLOR_RESET);
}


int todoui_draw(TodoUI* u)
{
    console_clear_screen();
    print_title(u);
    draw_line(u, TOP_OFFSET-1);
    cursor_position(TOP_OFFSET, u->minpos.col);

    for (int i=u->scroll_state; i<MIN(u->todos->n_todos, PAGE_SIZE+u->scroll_state); i++) {
        print_todo(u, i);
    }

    draw_line(u, u->cpos.line-1);
    cursor_position(u->cpos.line, u->cpos.col);
    return 0;
}


int todoui_mv_up(TodoUI* u, int n)
{
    // check if scroll is required
    if (u->vcpos.line-n < 0 && u->scroll_state > 0) {
        u->vcpos.line = 0;
        u->scroll_state = MAX(u->scroll_state - n, 0);
        todoui_draw(u);
    }
    else if (u->vcpos.line >= n) {
        cursor_position(u->vcpos.line+TOP_OFFSET, u->minpos.col);
        u->vcpos.line -= n;
        print_todo(u, u->vcpos.line+u->scroll_state+n);
        cursor_position(u->vcpos.line+TOP_OFFSET, u->minpos.col);
        print_todo(u, u->vcpos.line+u->scroll_state);
    }

    cursor_position(u->cpos.line, u->cpos.col);
    return 0;
}

int todoui_mv_down(TodoUI* u, int n)
{
    // check if scroll is required
    if (u->vcpos.line+n >= PAGE_SIZE && u->todos->n_todos > (u->scroll_state+PAGE_SIZE)) {
        u->vcpos.line = PAGE_SIZE-1;
        u->scroll_state = MIN(u->scroll_state + n, u->todos->n_todos-1);
        todoui_draw(u);
    }
    else if (u->vcpos.line+u->scroll_state+n < u->todos->n_todos) {
        cursor_position(u->vcpos.line+TOP_OFFSET, u->minpos.col);
        u->vcpos.line += n;
        print_todo(u, u->vcpos.line+u->scroll_state-n);
        cursor_position(u->vcpos.line+TOP_OFFSET, u->minpos.col);
        print_todo(u, u->vcpos.line+u->scroll_state);

    }

    cursor_position(u->cpos.line, u->cpos.col);
    return 0;
}

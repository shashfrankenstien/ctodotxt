#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "todo.h"
#include "todoui.h"
#include "ansictrl.h"


#define TOP_OFFSET 4 // includes 3 lines for the title and 1 line for border
#define PAGE_SIZE 15 // number of todos to display per page

#define HEADER_LINE 2
#define FOOTER_LINE TOP_OFFSET+PAGE_SIZE+1


static void print_todo(TodoUI* u, int todo_idx)
{
    console_clear_line();
    Todo* t = &u->todos->todos[todo_idx];

    char* color_cd;
    switch(t->priority) {
        case 'A':
            color_cd = COLOR_YELLOW;
            break;
        case 'B':
            color_cd = COLOR_GREEN;
            break;
        default:
            color_cd = COLOR_RESET;
            break;
    }

    if (u->vcpos.line + u->scroll_state == todo_idx) {
        printf("%s%s%s%d - %s%s\n", color_cd, COLOR_BOLD, COLOR_INVERSE, t->tid, t->raw_todo, COLOR_RESET);
    }
    else {
        printf("%s%d - %s%s\n", color_cd, t->tid, t->raw_todo, COLOR_RESET);
    }
}

static void draw_line(TodoUI* u, int on_line)
{
    cursor_position(on_line, u->minpos.col);
    char c = '-';
    printf(COLOR_GRAY);
    for (int i = u->minpos.col; i < u->maxpos.col; i ++) {
        putchar(c);
    }
    printf(COLOR_RESET);
}

static void print_header(TodoUI* u)
{
    // int col = (u->maxpos.col - u->minpos.col - strlen(u->title)) / 2; // centered
    int col = 2;
    cursor_position(HEADER_LINE, col);
    printf(COLOR_YELLOW);
    printf("%s", u->title);
    int rhs_col = u->maxpos.col - strlen(u->todopath);
    cursor_position(HEADER_LINE, rhs_col);
    printf("%s", u->todopath);
    printf(COLOR_RESET);
}

static void print_footer_left(TodoUI* u, const char* fmt, ...)
{
    cursor_position(FOOTER_LINE, u->minpos.col);
    printf(COLOR_YELLOW);
    va_list arg;
    va_start(arg, fmt);
    vprintf(fmt, arg);
    va_end(arg);
    printf(COLOR_RESET);
}

static void print_footer_right(TodoUI* u, const char* fmt, ...)
{
    va_list arg;
    char msg[100]; // arbitrary size to cover most cases
    va_start(arg, fmt);
    vsprintf(msg, fmt, arg);
    va_end(arg);
    // int col = (u->maxpos.col - u->minpos.col - strlen(u->title)) / 2; // centered
    int rhs_col = u->maxpos.col - strlen(msg);
    cursor_position(FOOTER_LINE, rhs_col-5);
    console_clear_line();
    cursor_position(FOOTER_LINE, rhs_col);
    printf(COLOR_YELLOW);
    printf("%s", msg);
    printf(COLOR_RESET);
}



TodoUI todoui_init(TodoArray* t, char* title, char* todopath)
{
    int lines, cols;
    get_console_size(&lines, &cols);
    // printf("ANSII codes: %s\n", (supports_ansii()) ? "TRUE" : "FALSE");
    // printf("Size: %d x %d\n", cols, lines);

    return (TodoUI){
        .title = title,
        .todopath = todopath,
        .todos=t,
        .cpos=(struct curpos){1,FOOTER_LINE+1},
        .vcpos=(struct curpos){0,0},
        .minpos=(struct curpos){1,1},
        .maxpos=(struct curpos){cols,lines},
        .scroll_state = 0
    };
}


int todoui_draw(TodoUI* u)
{
    console_clear_screen();
    print_header(u);
    draw_line(u, TOP_OFFSET-1);
    cursor_position(TOP_OFFSET, u->minpos.col);

    for (int i=u->scroll_state; i<MIN(u->todos->n_todos, PAGE_SIZE+u->scroll_state); i++) {
        print_todo(u, i);
    }

    draw_line(u, FOOTER_LINE-1);
    print_footer_left(u, "%s", "mode:default");
    print_footer_right(u, "%d/%d", u->vcpos.line+u->scroll_state+1, u->todos->n_todos);
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

    print_footer_right(u, "%d/%d", u->vcpos.line+u->scroll_state+1, u->todos->n_todos);
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

    print_footer_right(u, "%d/%d", u->vcpos.line+u->scroll_state+1, u->todos->n_todos);
    cursor_position(u->cpos.line, u->cpos.col);
    return 0;
}

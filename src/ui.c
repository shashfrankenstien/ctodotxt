#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "todo.h"
#include "ui.h"
#include "ansictrl.h"


#define TOP_OFFSET 5 // includes 3 lines for the title, 1 blank line and 1 line for column header
#define PAGE_SIZE 15 // number of todos to display on screen

#define HEADER_LINE 2
#define FOOTER_LINE TOP_OFFSET+PAGE_SIZE+1

#define NOTE_WIDTH_FACTOR 1/2.2

#if PLATFORM_WIN == 1
    #define H_LINE_CHAR "_"
    #define V_LINE_CHAR "|"
#else
    #define H_LINE_CHAR "\u2500"
    #define V_LINE_CHAR "\u2502"
#endif

#define STD_DATE_WIDTH 7
#define STD_DATE_FMT "%d-%b"

#define NOTE_TAG_WIDTH 10 // context or project name


static void fmt_date_std(char* buf, const int size, struct tm* dt) {
    if (dt->tm_year==0)
        snprintf(buf, size, "%-*s", size, " ");
    else {
        time_t rawtime;
        struct tm * timeinfo;
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );

        if (timeinfo->tm_year != dt->tm_year)
            strftime(buf, size, "%Y", dt);
        else
            strftime(buf, size, STD_DATE_FMT, dt);
    }
}


static void fmt_due_date(char* buf, const int size, struct tm* dt) {
    if (dt->tm_year==0)
        snprintf(buf, size, "%-*s", size, " ");
    else {
        time_t dt_stamp = mktime(dt);
        time_t now_stamp;
        time (&now_stamp);
        int hours = (dt_stamp-now_stamp)/60/60;
        int days = hours / 24;

        if (hours < -47)
            snprintf(buf, size, "%d days ago", abs(days));
        else if (hours < -23)
            snprintf(buf, size, "yesterday");
        else if (hours <= 0)
            snprintf(buf, size, "today");
        else if (hours <= 23)
            snprintf(buf, size, "tomorrow");
        else
            snprintf(buf, size, "in %d days", days);
    }
}


static void print_todo(TodoUI* u, int todo_idx)
{
    console_clear_line();
    Todo* t = &u->todos->todos[todo_idx];

    const char* color_cd;
    switch(t->priority) {
        case 'A':
            color_cd = COLOR_BOLD COLOR_RED;
            break;
        case 'B':
            color_cd = COLOR_BOLD COLOR_GREEN;
            break;
        case 'C':
            color_cd = COLOR_BOLD COLOR_BLUE;
            break;
        default:
            color_cd = COLOR_LIGHT_GRAY;
            break;
    }

    printf("%s %3d"V_LINE_CHAR, COLOR_GRAY, t->tid);
    if (u->vcpos.line + u->scroll_state == todo_idx)
        printf("%s"COLOR_BOLD COLOR_INVERSE, color_cd);
    else
        printf("%s", color_cd);

    char created_dt[STD_DATE_WIDTH];
    fmt_date_std(created_dt, STD_DATE_WIDTH, &t->created_date);

    char fin_dt[STD_DATE_WIDTH];
    fmt_date_std(fin_dt, STD_DATE_WIDTH, &t->finished_date);

    char due_dt[15];
    if (t->finished) {
        due_dt[0] = ' ';
        due_dt[1] = '\0';
    }
    else {
        fmt_due_date(due_dt, 15, &t->due_date);
    }

    char pri[4] = "   ";
    if (t->priority!=NO_PRIORITY)
        snprintf(pri, 4, "(%c)", t->priority);

    int note_width = (u->maxpos.col - u->minpos.col) * NOTE_WIDTH_FACTOR;

    char* todo_copy = calloc(note_width, sizeof(char));
    strncpy(todo_copy, t->todo, note_width);

    if (strlen(t->todo) >= note_width) {
        for (int i=1; i<=2; i++)
            todo_copy[note_width-i] = '.';
    }

    printf(" %c %s  %-*.*s %-*.*s "V_LINE_CHAR" %-*.*s"V_LINE_CHAR" %-*.*s %-15.15s"COLOR_RESET COLOR_GRAY V_LINE_CHAR COLOR_RESET"\n",
        t->finished ? 'x' : ' ', pri,
        STD_DATE_WIDTH, STD_DATE_WIDTH, created_dt,
        STD_DATE_WIDTH, STD_DATE_WIDTH, fin_dt,
        note_width, note_width, todo_copy,
        NOTE_TAG_WIDTH, NOTE_TAG_WIDTH, t->contexts[0],
        due_dt
    );

    free(todo_copy);
}


static void print_columns_names(TodoUI* u)
{
    cursor_position(TOP_OFFSET-1, u->minpos.col+5); // position cursor past the index column
    int note_width = (u->maxpos.col - u->minpos.col) * NOTE_WIDTH_FACTOR;
    printf(
        COLOR_UNDERLINED
        COLOR_GRAY
        " F  P   %-*s %-*s   %-*s  %-*s %-15s"
        COLOR_RESET, // fmt ends
        STD_DATE_WIDTH, "add",
        STD_DATE_WIDTH, "fin",
        note_width, "todo",
        NOTE_TAG_WIDTH, "context",
        "due"
    );
}


static void draw_line(TodoUI* u, int on_line)
{
    cursor_position(on_line, u->minpos.col);
    printf(COLOR_GRAY);
    for (int i = u->minpos.col; i < u->maxpos.col; i ++) {
        printf(H_LINE_CHAR);
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
    cursor_position(FOOTER_LINE, u->minpos.col + (u->maxpos.col / 2));
    console_clear_sol();
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
    cursor_position(FOOTER_LINE, u->minpos.col + (u->maxpos.col / 2));
    console_clear_eol();

    va_list arg;
    char msg[100]; // arbitrary size to cover most cases
    va_start(arg, fmt);
    vsnprintf(msg, 100, fmt, arg);
    va_end(arg);
    // int col = (u->maxpos.col - u->minpos.col - strlen(u->title)) / 2; // centered
    int rhs_col = u->maxpos.col - strlen(msg);
    cursor_position(FOOTER_LINE, rhs_col);
    printf(COLOR_YELLOW);
    printf("%s", msg);
    printf(COLOR_RESET);
}


static void print_default_footer(TodoUI* u)
{
    int cur_idx = u->vcpos.line+u->scroll_state;
    print_footer_left(u, "%s", u->todos->todos[cur_idx].todo);
    print_footer_right(u, "%d/%d", cur_idx+1, u->todos->n_todos);
}


TodoUI todoui_init(TodoArray* t, char* title, char* todopath)
{
    return (TodoUI){
        .title = title,
        .todopath = todopath,
        .todos=t,
        .cpos=(struct curpos){1,FOOTER_LINE+1},
        .vcpos=(struct curpos){0,0},
        .minpos=(struct curpos){1,1},
        // .maxpos=(struct curpos){cols,lines},
        .scroll_state = 0
    };
}


int todoui_draw(TodoUI* u)
{
    int lines, cols;
    get_console_size(&lines, &cols);
    u->maxpos.line = lines;
    u->maxpos.col = cols;

    console_clear_screen();
    print_header(u);
    print_columns_names(u);
    cursor_position(TOP_OFFSET, u->minpos.col);

    for (int i=u->scroll_state; i<MIN(u->todos->n_todos, PAGE_SIZE+u->scroll_state); i++) {
        print_todo(u, i);
    }

    draw_line(u, FOOTER_LINE-1);
    print_default_footer(u);
    todoui_reset_cursor(u);
    return 0;
}


static int todoui_vc_nav(TodoUI* u, int n)
{
    int orig_scroll = u->scroll_state;
    int orig_todo_idx = u->vcpos.line + u->scroll_state;

    int new_todo_idx = MAX(MIN(orig_todo_idx + n, u->todos->n_todos-1), 0);
    int new_vpos = MAX(MIN(u->vcpos.line + n, MIN(PAGE_SIZE-1, u->todos->n_todos-1)), 0);
    int new_scroll = MAX(new_todo_idx - new_vpos, 0);

    if (new_scroll > orig_scroll) { // gotta scroll down and set vpos to PAGE_SIZE
        u->scroll_state = new_scroll;
        u->vcpos.line = PAGE_SIZE-1;
        todoui_draw(u);
    }
    else if (new_scroll < orig_scroll) { // gotta scroll up and set vpos to 0
        u->scroll_state = new_scroll;
        u->vcpos.line = 0;
        todoui_draw(u);
    }
    else {
        cursor_position(u->vcpos.line+TOP_OFFSET, u->minpos.col);
        u->vcpos.line = new_vpos;
        print_todo(u, orig_todo_idx);
        cursor_position(u->vcpos.line+TOP_OFFSET, u->minpos.col);
        print_todo(u, new_todo_idx);
    }

    print_default_footer(u);
    todoui_reset_cursor(u);
    return 0;
}


int todoui_vc_down(TodoUI* u, int n)
{
    return todoui_vc_nav(u, n);
}


int todoui_vc_up(TodoUI* u, int n)
{
    return todoui_vc_nav(u, n*-1);
}


void todoui_reset_cursor(TodoUI* u)
{
    cursor_position(u->cpos.line, u->cpos.col);
}

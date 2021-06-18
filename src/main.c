#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "todo.h"
#include "ui.h"
#include "ansictrl.h"



// file reader
typedef void (*getline_cb_t)(void* data, char* line); // callback type for iterlines over a file

void iterlines(FILE* fp, getline_cb_t cb, void* data);
void read_conf(void* todos, char* line); // is of type getline_cb_t




// cli commands

typedef enum {
    NOOP = -1,
    WRITE,
    QUIT,
    WRITE_QUIT,
    SORT,
    FILTER,
} cmd_var;


typedef struct {
    char* cmd_str;
    cmd_var cmd;
} cmd_lookup_t;


static cmd_lookup_t cmd_lookup_table[] = {
    {"q", QUIT},
    {"quit", QUIT},
    {"w", WRITE},
    {"write", WRITE},
    {"wq", WRITE_QUIT},
    {"s", SORT},
    {"sort", SORT},
    {"f", FILTER},
    {"filter", FILTER},
};

#define NCMDS (sizeof(cmd_lookup_table)/sizeof(cmd_lookup_t))

cmd_var todoui_lookup_cmd(char* cmd_str);
int command_mode(TodoUI* ui);
void perform_sort_cmd(TodoUI* ui);



// impl

cmd_var todoui_lookup_cmd(char* cmd_str)
{
    for (int i=0; i < NCMDS; i++) {
        if (strcmp(cmd_lookup_table[i].cmd_str, cmd_str) == 0)
            return cmd_lookup_table[i].cmd;
    }
    return NOOP;
}



int command_mode(TodoUI* ui)
{
    char ch;
    bool multibyte;

    char cmd[256];
    int cmd_idx = 0;

    bool done = false;
    while (!done) {
        ch = readkey(&multibyte);
        if (multibyte) {
        }
        else {
            switch(ch) {
                case ENTER:
                    cmd[cmd_idx] = '\0'; // finalize
                    done = true;
                    break;

                case ESCAPE:
                    cmd[0] = '\0'; // clear command
                    done = true;
                    break;

                case BACK_SPACE:
                    putchar('\b');
                    putchar(' ');
                    putchar('\b');
                    if (cmd_idx>0)
                        cmd[--cmd_idx] = '\0'; // clear char
                    else
                        done=true;
                    break;

                default:
                    putchar(ch);
                    cmd[cmd_idx++] = ch;
                    break;
            }
        }
    }
    todoui_reset_cursor(ui);
    console_clear_line();

    int quit = 0;
    switch(todoui_lookup_cmd(cmd)) {
        case QUIT:
            quit = 1;
            break;

        case SORT:
            perform_sort_cmd(ui);
            break;

        default:
            break;
    }

    return quit;
}


void perform_sort_cmd(TodoUI* ui)
{
    char ch;
    bool multibyte;

    bool done = false;
    while (!done) {
        printf(COLOR_YELLOW);
        printf("\nSort by:\nf: finished date\nc: created date\np: priority\nd: due date\ni: ID\nPress ESC key to exit sort mode.");
        printf(COLOR_RESET);
        todoui_reset_cursor(ui);

        ch = readkey(&multibyte);
        if (!multibyte) {
            switch(ch) {
                case ESCAPE:
                    todoui_draw(ui);
                    done = true;
                    break;

                case 'f':
                    todoarray_sort(ui->todos, FINISHED_DATE);
                    todoui_draw(ui);
                    break;

                case 'c':
                    todoarray_sort(ui->todos, CREATED_DATE);
                    todoui_draw(ui);
                    break;

                case 'p':
                    todoarray_sort(ui->todos, PRIORITY);
                    todoui_draw(ui);
                    break;

                case 'd':
                    todoarray_sort(ui->todos, DUE_DATE);
                    todoui_draw(ui);
                    break;

                case 'i':
                    todoarray_sort(ui->todos, ID);
                    todoui_draw(ui);
                    break;
            }
        }
    }
}



// file reader
void iterlines(FILE* fp, getline_cb_t cb, void* data)
{
#if PLATFORM_WIN == 1
	char line[1000];
	int size = 1000 * sizeof(char);
	while(fgets(line, size, fp) != NULL) {
		cb(data, line);
	}
#else
	char *line = (char*) malloc(sizeof(char) * 1000);
	size_t size = 1000 * sizeof(char);
	while(getline(&line, &size, fp) != EOF) {
		cb(data, line);
	}
	free(line);
#endif
}

void read_conf(void* todos, char* line)
{
    if (line[0]=='#') return; // commented

    int cpy_len;
    char filename[256];// = "todo.txt";

    if (line[strlen(line)-1] == '\n')
        cpy_len = strlen(line)-1;
    else
        cpy_len = strlen(line);

    strncpy(filename, line, cpy_len);
    filename[cpy_len] = '\0';

    printf("%s\n", filename);
    FILE* fp = fopen(filename, "r");
    if (fp == NULL)
        return;

    iterlines(fp, todoarray_add_cb, (TodoArray*)todos);
    fclose(fp);
}




int main (int argc, char *argv[])
{
    TodoArray todos = todoarray_init();

    {
        FILE* fp = fopen("todo.conf", "r");
        if (fp == NULL)
            goto error;

        iterlines(fp, read_conf, &todos);
        fclose(fp);
    }

    // sort test
    todoarray_sort(&todos, PRIORITY);
    todoarray_sort(&todos, FINISHED_DATE);

    TodoUI ui = todoui_init(&todos, argv[0], "todo.txt");

    // // filter test
    // {
    //     printf("\n\n");
    //     TodoSlice slice = todoarray_filter(&todos, "MDOTS");
    //     for (int i=0; i<slice.n_todos; i++) {
    //         Todo* t = slice.todos[i];

    //         printf("%d - %s%s%s%s%s\n", t->tid, COLOR_BOLD, COLOR_YELLOW, COLOR_INVERSE, t->raw_todo, COLOR_RESET);
    //     }
    //     todoslice_release(&slice);
    // }

    todoui_draw(&ui);

    char ch;
    bool multibyte;

    bool done = false;
    while (!done) {
        ch = readkey(&multibyte);
        if (multibyte) {
            switch(ch) {
                case UP:
                    todoui_vc_up(&ui, 1);
                    break;

                case DOWN:
                    todoui_vc_down(&ui, 1);
                    break;

                // case LEFT:
                //     cursor_mv_left(1);
                //     break;

                // case RIGHT:
                //     cursor_mv_right(1);
                //     break;

                default:
                    break;
            }
        } else {
            int cmd_res;
            switch(ch) {
                case ENTER:
                    todoui_draw(&ui);
                    break;

                case ESCAPE:
                    todoui_reset_cursor(&ui);
                    console_clear_line();
                    // done = true;
                    break;

                case ':':
                    putchar(ch);
                    cmd_res = command_mode(&ui);
                    if (cmd_res!=0)
                        done = true;
                    break;

                default:
                    putchar(ch);
                    break;
            }
        }
    }
    printf("\nExit!\n");
    todoarray_release(&todos);
    return 0;

error:
    printf("File not found!\n");
    return -1;
}

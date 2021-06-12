#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "todo.h"
#include "todoui.h"
#include "ansictrl.h"


typedef void (*getline_cb_t)(void* data, char* line);


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
    bool special;

    while ((ch = readkey(&special)) != ESCAPE) {
        if (special) {
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

            switch(ch) {
                case ENTER:
                    todoui_draw(&ui);
                    break;

                case 'f':
                    todoarray_sort(&todos, FINISHED_DATE);
                    todoui_draw(&ui);
                    break;

                case 'c':
                    todoarray_sort(&todos, CREATED_DATE);
                    todoui_draw(&ui);
                    break;

                case 'p':
                    todoarray_sort(&todos, PRIORITY);
                    todoui_draw(&ui);
                    break;

                case 'd':
                    todoarray_sort(&todos, DUE_DATE);
                    todoui_draw(&ui);
                    break;

                case 'i':
                    todoarray_sort(&todos, ID);
                    todoui_draw(&ui);
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

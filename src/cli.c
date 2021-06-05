#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "todo.h"
#include "ansictrl.h"
#include "keys.h"


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




int main (int argc, char *argv[])
{
    FILE* fp = fopen("todo.txt", "r");
    if (fp == NULL)
        goto error;

    TodoArray todos = todoarray_init();
    iterlines(fp, todoarray_add_cb, &todos);
    fclose(fp);

    console_clear_screen();

    printf("ANSII codes: %s\n", (supports_ansii()) ? "TRUE" : "FALSE");

    int lines, cols;
    get_console_size(&lines, &cols);
    printf("Size: %d x %d\n", cols, lines);

    // sort test
    todoarray_sort(&todos, PRIORITY);
    for (int i=0; i<todos.n_todos; i++) {
        printf("%d - %s\n", todos.todos[i].tid, todos.todos[i].raw_todo);
    }

    // filter test
    {
        printf("\n\n");
        TodoSlice slice = todoarray_filter(&todos, "MDOTS");
        for (int i=0; i<slice.n_todos; i++) {
            Todo* t = slice.todos[i];

            char* raw_color = calloc(strlen(t->raw_todo)+strlen(COLORIZE_TMPL), sizeof(char));
            colorize(raw_color, t->raw_todo, YELLOW, true);
            printf("%d - %s\n", t->tid, raw_color);

            free(raw_color);
        }
        todoslice_release(&slice);
    }

    todoarray_release(&todos);

    char ch;
    bool special;

    while ((ch = readkey(&special)) != ESCAPE) {
        if (special) {
            switch(ch) {
                case UP:
                    cursor_mv_up(1);
                    break;

                case DOWN:
                    cursor_mv_down(1);
                    break;

                case LEFT:
                    cursor_mv_left(1);
                    break;

                case RIGHT:
                    cursor_mv_right(1);
                    break;

                case 'd':
                    console_clear_line();
                    break;

                default:
                    break;
            }
        } else {
            printf("def %c - %d \n", ch, ch);
        }
    }
    return 0;

error:
    printf("File not found!\n");
    return -1;
}

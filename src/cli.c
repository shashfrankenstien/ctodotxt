#include <stdlib.h>
#include <stdio.h>

#include "todo.h"
#include "keys.h"


typedef void (*getline_cb_t)(void* data, char* line);


void iterlines(FILE* fp, getline_cb_t cb, void* data)
{
#if (defined _WIN32 || defined _WIN64 || defined __WINDOWS__)
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


void console_clear_screen() {
#if (defined _WIN32 || defined _WIN64 || defined __WINDOWS__)
    system("cls");
#else
    system("clear");
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
            printf("%d - %s\n", t->tid, t->raw_todo);
        }
        todoslice_release(&slice);
    }

    todoarray_release(&todos);

    char ch;
    while ((ch = readkey())!=27) {
        printf("%c - %d \n", ch, ch);
    }
    return 0;

error:
    printf("File not found!\n");
    return -1;
}

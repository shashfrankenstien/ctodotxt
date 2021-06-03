#include <stdlib.h>
#include <stdio.h>

#include "todo.h"


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
	FILE *fp = fopen(file, "r");
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

    TodoVect todos;
    todovect_create(&todos, MAX_TODOS);

    iterlines(fp, todovect_add_cb, &todos);
    fclose(fp);

    // sort test
    todovect_sort(&todos, PRIORITY);
    for (int i=0; i<todos.n_todos; i++) {
        printf("%d - %s\n", todos.todos[i].tid, todos.todos[i].raw_todo);
    }

    // filter test
    {
        printf("\n\n");
        TodoSlice todos_filtered = todovect_filter(&todos, "GC");
        for (int i=0; i<todos_filtered.n_todos; i++) {
            Todo* t = todos_filtered.todos[i];
            printf("%d - %s\n", t->tid, t->raw_todo);
        }
    }

    todovect_release(&todos);
    return 0;

error:
    printf("File not found!\n");
    return -1;
}

#include <stdlib.h>
#include <stdio.h>

#include "todo.h"


int read_todo_file(TodoVect* todos, FILE* fp)
{
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, fp)) != -1) {
        Todo* t = &todos->todos[todos->n_todos];
        if(todo_parse(t, line)==0) { // success!
            todos->n_todos++;
        }
    }
    if (line)
        free(line);
    return 0;
}


int main (int argc, char *argv[])
{


    FILE* fp = fopen("todo.txt", "r");
    if (fp == NULL)
        goto error;

    TodoVect todos;
    todovect_create(&todos, MAX_TASKS);

    read_todo_file(&todos, fp);
    fclose(fp);

    // sort test
    todovect_sort(&todos, DUE_DATE);
    for (int i=0; i<todos.n_todos; i++) {
        printf("%d - %s\n", todos.todos[i].tid, todos.todos[i].raw_todo);
    }

    // filter test
    {
        printf("\n\n");
        TodoVect todos_filtered;
        todovect_create(&todos_filtered, todos.n_todos);
        todovect_filter(&todos_filtered, &todos, "@");
        for (int i=0; i<todos_filtered.n_todos; i++) {
            printf("%d - %s\n", todos_filtered.todos[i].tid, todos_filtered.todos[i].raw_todo);
        }
        todovect_release(&todos_filtered);
    }

    todovect_release(&todos);
    return 0;

error:
    printf("File not found!\n");
    return -1;
}

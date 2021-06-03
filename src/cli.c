#include <stdlib.h>
#include <stdio.h>

#include "todotxt.h"


int read_todo_file(Todotxt* todos, FILE* fp)
{
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, fp)) != -1) {
        Task* t = &todos->tasks[todos->n_tasks];
        if(task_parse(t, line)==0) { // success!
            todos->n_tasks++;
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

    Todotxt todos;
    todotxt_create(&todos, MAX_TASKS);

    read_todo_file(&todos, fp);
    fclose(fp);

    // sort test
    todotxt_sort(&todos, DUE_DATE);
    for (int i=0; i<todos.n_tasks; i++) {
        printf("%d - %s\n", todos.tasks[i].tid, todos.tasks[i].raw_todo);
    }

    printf("\n\n");

    Todotxt todos_filtered;
    todotxt_create(&todos_filtered, todos.n_tasks);
    todotxt_filter(&todos_filtered, &todos, "@");
    for (int i=0; i<todos_filtered.n_tasks; i++) {
        printf("%d - %s\n", todos_filtered.tasks[i].tid, todos_filtered.tasks[i].raw_todo);
    }

    todotxt_release(&todos);
    todotxt_release(&todos_filtered);
    return 0;

error:
    printf("File not found!\n");
    return -1;
}

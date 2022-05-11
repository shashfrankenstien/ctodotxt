#include <stdlib.h> // for calloc, realloc and free
#include <stdio.h>

#include "todo.h"


#ifndef ARRAY_ALLOC_SIZE
    #define ARRAY_ALLOC_SIZE 50
#endif

TodoArray todoarray_init()
{
    TodoArray t;
    t.todos = calloc(ARRAY_ALLOC_SIZE, sizeof(Todo));
    t.n_todos = 0;

    t.view = NULL;
    t.n_view = 0;
    return t;
}

int todoarray_release(TodoArray* t)
{
    free(t->view);
    free(t->todos);
    return 0;
}



int todoarray_add(TodoArray* t, char* line)
{
    if (t->n_todos % ARRAY_ALLOC_SIZE == 0) { // realloc at multiple of ARRAY_ALLOC_SIZE value
        t->todos = realloc(t->todos, sizeof(Todo) * (t->n_todos + ARRAY_ALLOC_SIZE));
    }
    int res;
    if((res=todo_parse(&t->todos[t->n_todos], line))==0) { // success!
        t->n_todos++;
    }
    return res;
}

void todoarray_add_cb(void* obj, char* line)
{
    todoarray_add((TodoArray*)obj, line);
}

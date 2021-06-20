#include <stdlib.h> // for calloc, realloc and free
#include <stdio.h>

#include "todo.h"
#include "mergesort.h"


#ifndef ARRAY_ALLOC_SIZE
    #define ARRAY_ALLOC_SIZE 50
#endif

TodoArray todoarray_init()
{
    TodoArray t;
    t.todos = calloc(ARRAY_ALLOC_SIZE, sizeof(Todo));
    t.n_todos = 0;
    return t;
}

int todoarray_release(TodoArray* t)
{
    free(t->todos);
    return 0;
}

int todoslice_release(TodoSlice* t)
{
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

int todoarray_sort(TodoArray* t, TodoField field)
{
    msort_r(t->todos, t->n_todos, sizeof(Todo), todo_cmp_asc, &field);
    return 0;
}

int todoarray_sort_desc(TodoArray* t, TodoField field)
{
    msort_r(t->todos, t->n_todos, sizeof(Todo), todo_cmp_desc, &field);
    return 0;
}

TodoSlice todoarray_filter(TodoArray* src, const char* pattern)
{
    TodoSlice slc;
    slc.n_todos = 0;
    slc.todos = calloc(src->n_todos, sizeof(Todo*));

    for (int i=0; i<src->n_todos; i++) {
        if (todo_match(&src->todos[i], pattern)) {
            slc.todos[slc.n_todos++] = &src->todos[i];
        }
    }

    return slc;
}

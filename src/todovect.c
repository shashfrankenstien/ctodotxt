#include <stdlib.h> // for calloc, realloc and free

#include "sort_r.h"

#include "todo.h"


int todovect_create(TodoVect* t, int n)
{
    t->todos = calloc(n, sizeof(Todo));
    t->n_todos = 0;
    return 0;
}

int todovect_release(TodoVect* t)
{
    free(t->todos);
    return 0;
}

int todovect_sort(TodoVect* t, TodoField field)
{
    sort_r(t->todos, t->n_todos, sizeof(Todo), todo_cmp_asc, &field);
    return 0;
}

int todovect_sort_desc(TodoVect* t, TodoField field)
{
    sort_r(t->todos, t->n_todos, sizeof(Todo), todo_cmp_desc, &field);
    return 0;
}

TodoSlice todovect_filter(TodoVect* src, const char* pattern)
{
    TodoSlice slc;
    slc.n_todos = 0;

    for (int i=0; i<src->n_todos; i++) {
        if (todo_match(&src->todos[i], pattern)) {
            slc.todos[slc.n_todos++] = &src->todos[i];
        }
    }

    return slc;
}

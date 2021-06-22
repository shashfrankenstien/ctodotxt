#include <stdlib.h>
#include <stdbool.h>

#include "re.h"
#include "mergesort.h"
#include "todo.h"


static int slice_cmp_asc(const void* _t, const void* _oth, void* _field)
{
    return todo_cmp_asc(*(Todo**)_t, *(Todo**)_oth, _field);
}

static int slice_cmp_desc(const void* _t, const void* _oth, void* _field)
{
    return todo_cmp_desc(*(Todo**)_t, *(Todo**)_oth, _field);
}




void clear_slice(TodoArray* t)
{
    if (t->slice)
        free(t->slice);

    t->slice = calloc(t->n_todos, sizeof(Todo*));
    t->n_slice = 0;
}

int todoslice_create(TodoArray* t)
{
    clear_slice(t);
    for (int i=0; i<t->n_todos; i++) {
        t->slice[t->n_slice++] = &t->todos[i];
    }
    return 0;
}


int todoslice_sort(TodoArray* t, TodoField field)
{
    msort_r(t->slice, t->n_slice, sizeof(Todo*), slice_cmp_asc, &field);
    return 0;
}

int todoslice_sort_desc(TodoArray* t, TodoField field)
{
    msort_r(t->slice, t->n_slice, sizeof(Todo*), slice_cmp_desc, &field);
    return 0;
}


int todoslice_search(TodoArray* t, const char* pattern)
{
    clear_slice(t);

    for (int i=0; i<t->n_todos; i++) {
        if (todo_match(&t->todos[i], pattern)) {
            t->slice[t->n_slice++] = &t->todos[i];
        }
    }
    return 0;
}

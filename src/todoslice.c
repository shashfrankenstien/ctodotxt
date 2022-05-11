#include <stdlib.h>
#include <stdbool.h>

#include "re.h"
#include "mergesort.h"
#include "todo.h"


static int view_cmp_asc(const void* _t, const void* _oth, void* _field)
{
    return todo_cmp_asc(*(Todo**)_t, *(Todo**)_oth, _field);
}

static int view_cmp_desc(const void* _t, const void* _oth, void* _field)
{
    return todo_cmp_desc(*(Todo**)_t, *(Todo**)_oth, _field);
}

static void clear_view(TodoArray* t)
{
    if (t->view)
        free(t->view);

    t->view = calloc(t->n_todos, sizeof(Todo*));
    t->n_view = 0;
}



int todoview_create(TodoArray* t)
{
    clear_view(t);
    for (int i=0; i<t->n_todos; i++) {
        t->view[t->n_view++] = &t->todos[i];
    }
    // apply default sorting
    todoview_sort(t, PRIORITY);
    todoview_sort(t, FINISHED_DATE);
    return 0;
}


int todoview_sort(TodoArray* t, TodoField field)
{
    msort_r(t->view, t->n_view, sizeof(Todo*), view_cmp_asc, &field);
    return 0;
}

int todoview_sort_desc(TodoArray* t, TodoField field)
{
    msort_r(t->view, t->n_view, sizeof(Todo*), view_cmp_desc, &field);
    return 0;
}


int todoview_search(TodoArray* t, const char* pattern)
{
    clear_view(t);

    for (int i=0; i<t->n_todos; i++) {
        if (todo_match(&t->todos[i], pattern)) {
            t->view[t->n_view++] = &t->todos[i];
        }
    }
    return 0;
}

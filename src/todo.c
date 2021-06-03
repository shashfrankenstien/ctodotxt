#define _GNU_SOURCE
#include <stdlib.h> // for calloc, realloc and free

#include "sort_r.h"

#include "todotxt.h"



int todotxt_create(Todotxt* t, int n)
{
    t->tasks = calloc(n, sizeof(Task));
    t->n_tasks = 0;
    return 0;
}


int todotxt_release(Todotxt* t)
{
    free(t->tasks);
    return 0;
}

int todotxt_sort(Todotxt* t, TaskField field)
{
    sort_r(t->tasks, t->n_tasks, sizeof(Task), task_cmp_asc, &field);
    return 0;
}

int todotxt_sort_desc(Todotxt* t, TaskField field)
{
    sort_r(t->tasks, t->n_tasks, sizeof(Task), task_cmp_desc, &field);
    return 0;
}

int todotxt_filter(Todotxt* dest, Todotxt* src, const char* pattern)
{
    for (int i=0; i<src->n_tasks; i++) {
        if (task_match(&src->tasks[i], pattern)) {
            dest->tasks[dest->n_tasks++] = src->tasks[i];
        }
    }

    return (dest->n_tasks > 0) ? 0 : -1;
}

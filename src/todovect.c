#include <stdlib.h> // for calloc, realloc and free

/* These guards are required for sort_r.h while using TDM-GCC on Windows
*  Waiting for resolution of issue https://github.com/noporpoise/sort_r/issues/14
*/
#ifdef __MINGW32__
    #define __REMOVED__MINGW32__
    #undef __MINGW32__
#endif

#include "sort_r.h"

#ifdef __REMOVED__MINGW32__
    #define __MINGW32__ 1
    #undef __REMOVED__MINGW32__
#endif


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

int todovect_add(TodoVect* t, char* line)
{
    int res;
    if((res=todo_parse(&t->todos[t->n_todos], line))==0) { // success!
        t->n_todos++;
    }
    return res;
}

void todovect_add_cb(void* obj, char* line)
{
    todovect_add((TodoVect*)obj, line);
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

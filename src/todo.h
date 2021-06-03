#pragma once

#include <time.h>
#include <stdbool.h>

#ifndef MAX_LINE_LENGTH
    #define MAX_LINE_LENGTH 512
#endif // MAX_LINE_LENGTH

#ifndef MAX_TAGS
    #define MAX_TAGS 20 // projects or contexts
#endif // MAX_TAGS

#ifndef MAX_TODOS
    #define MAX_TODOS 512
#endif // MAX_TODOS

#define DATE_REGEX "\\d\\d\\d\\d-\\d\\d-\\d\\d"
#define DATE_FMT "%Y-%m-%d"
#define DUEDATE_REGEX "due:\\d\\d\\d\\d-\\d\\d-\\d\\d"
#define DUEDATE_FMT "due:%Y-%m-%d"


#define CONTEXT_REGEX "@\\w+"
#define PROJECT_REGEX "\\+\\w+"

#define PRIORITY_REGEX "([A-Z])"
#define PRIORITY_KV_REGEX "pri:[A-Z]"
#define NO_PRIORITY '^' // works better than '\0' for sorting

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

#define CAP_LENGTH(X) MIN(X, MAX_LINE_LENGTH)


typedef struct {
    int tid;
    char raw_todo[MAX_LINE_LENGTH];
    char priority;
    char todo[MAX_LINE_LENGTH];
    char projects[MAX_TAGS][MAX_LINE_LENGTH];
    char contexts[MAX_TAGS][MAX_LINE_LENGTH];

    bool finished;
    struct tm created_date;
    struct tm due_date;
    struct tm finished_date;
} Todo;


typedef enum {
    ID, PRIORITY, FINISHED, CREATED_DATE, DUE_DATE, FINISHED_DATE
} TodoField;


int todo_parse(Todo* t, char* line);
int todo_rebuild(Todo* t, char* out);

int todo_cmp_asc(const void* t, const void* oth, void* field);
int todo_cmp_desc(const void* t, const void* oth, void* field);

bool todo_match(Todo* t, const char* pat);





typedef struct {
    Todo* todos;
    int n_todos;
} TodoVect;

int todovect_create(TodoVect* t, int n);
int todovect_release(TodoVect* t);


typedef struct {
    Todo* todos[MAX_TODOS];
    int n_todos;
} TodoSlice;


int todovect_sort(TodoVect* t, TodoField field);
int todovect_sort_desc(TodoVect* t, TodoField field);

TodoSlice todovect_filter(TodoVect* src, const char* pattern);

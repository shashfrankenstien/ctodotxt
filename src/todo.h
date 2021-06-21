#pragma once

#include <time.h>
#include <stdbool.h>

#ifndef MAX_LINE_LENGTH
    #define MAX_LINE_LENGTH 256
#endif

#ifndef MAX_TAGS
    #define MAX_TAGS 20 // represents projects or contexts
#endif


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
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

#define CAP_LENGTH(X) MIN(X, MAX_LINE_LENGTH)


// single todo
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

// sorting and filtering helpers
int todo_cmp_asc(const void* t, const void* oth, void* field);
int todo_cmp_desc(const void* t, const void* oth, void* field);
bool todo_match(Todo* t, const char* pat);



// todo array
typedef struct {
    Todo* todos;
    int n_todos;

    // slice contains pointers to TodoArray elements
    Todo** slice;
    int n_slice;
} TodoArray;


TodoArray todoarray_init();
int todoarray_release(TodoArray* t);

int todoarray_add(TodoArray* t, char* line);
void todoarray_add_cb(void* obj, char* line);


// slice operations
int todoslice_create(TodoArray* t);
// sorting and filtering
int todoslice_sort(TodoArray* t, TodoField field);
int todoslice_sort_desc(TodoArray* t, TodoField field);
int todoslice_filter(TodoArray* src, const char* pattern);

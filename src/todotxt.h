#pragma once

#include <time.h>
#include <stdbool.h>

#ifndef MAX_LINE_LENGTH
    #define MAX_LINE_LENGTH 512
#endif // MAX_LINE_LENGTH

#ifndef MAX_TAGS
    #define MAX_TAGS 20 // projects or contexts
#endif // MAX_TAGS

#ifndef MAX_TASKS
    #define MAX_TASKS 512
#endif // MAX_TASKS

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
} Task;


typedef enum {
    ID, PRIORITY, FINISHED, CREATED_DATE, DUE_DATE, FINISHED_DATE
} TaskField;


int task_parse(Task* t, char* line);
int task_rebuild(Task* t, char* out);

int task_cmp_asc(const void* t, const void* oth, void* field);
int task_cmp_desc(const void* t, const void* oth, void* field);

bool task_match(Task* t, const char* pat);



typedef struct {
    Task* tasks;
    int n_tasks;
} Todotxt;


int todotxt_create(Todotxt* t, int n);
int todotxt_release(Todotxt* t);

int todotxt_sort(Todotxt* t, TaskField field);
int todotxt_sort_desc(Todotxt* t, TaskField field);

int todotxt_filter(Todotxt* dest, Todotxt* src, const char* pattern);

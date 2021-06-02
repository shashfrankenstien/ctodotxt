#pragma once

#include <time.h>
#include <stdbool.h>

#ifndef MAX_LINE_LENGTH
    #define MAX_LINE_LENGTH 512
#endif // MAX_TAGS

#ifndef MAX_TAGS
    #define MAX_TAGS 20 // projects or contexts
#endif // MAX_TAGS


#define DATE_REGEX "\\d\\d\\d\\d-\\d\\d-\\d\\d"
#define DATE_FMT "%Y-%m-%d"
#define DUEDATE_REGEX "due:\\d\\d\\d\\d-\\d\\d-\\d\\d"
#define DUEDATE_FMT "due:%Y-%m-%d"


#define CONTEXT_REGEX "@\\w+"
#define PROJECT_REGEX "\\+\\w+"

#define PRIORITY_REGEX "([A-Z])"
#define PRIORITY_KV_REGEX "pri:[A-Z]"
#define NO_PRIORITY_CHARACTER '\0'

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

int parse_task(Task* t, char* l);
int rebuild_task(Task* t, char* o);

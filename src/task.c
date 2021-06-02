#define _XOPEN_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#include "re.h"

#include "task.h"


// private functions
static void constructor(Task* t);
static void break_at_new_line(char* l);


// impl
static void break_at_new_line(char* l)
{
    for (int i=0; i < CAP_LENGTH(strlen(l)); i++) {
        if (l[i] == '\n') {
            l[i] = '\0';
            break;
        }
    }
}


static void constructor(Task* t)
{
    static int tid = 0; // static int to act as id
    t->tid = tid++;

    t->raw_todo[0] = '\0';
    t->priority = NO_PRIORITY_CHARACTER;
    t->todo[0] = '\0';

    for (int i=0; i<MAX_TAGS; i++) {
        t->projects[i][0] = '\0';
        t->contexts[i][0] = '\0';
    }
    t->finished = false;

    t->created_date.tm_year = 0;
    t->due_date.tm_year = 0;
    t->finished_date.tm_year = 0;
}


int parse_task(Task* t, char* line)
{

    char *token;
    const char sep[2] = " ";
    int match, matchlen;
    int n_projects = 0, n_contexts = 0;

    break_at_new_line(line);

    constructor(t);
    strncpy(t->raw_todo, line, CAP_LENGTH(strlen(line)));

    /* walk through tokens */
    token = strtok(line, sep);
    if (token == NULL) return -1;
    if (strncmp(token, "x", strlen(token)) == 0) {
        t->finished = true;
        token = strtok(NULL, sep);
        if (token == NULL) return -1;
        match = re_match(DATE_REGEX, (const char*)token, &matchlen);
        if (match > -1) {
            strptime((const char*)token, "%Y-%m-%d", &t->finished_date);
            token = strtok(NULL, sep);
        }
    }

    if (token == NULL) return -1;

    // priority
    match = re_match(PRIORITY_REGEX, (const char*)token, &matchlen);
    if (match > -1) {
        t->priority = token[match+1];
        token = strtok(NULL, sep);
    }

    if (token == NULL) return -1;
    match = re_match(DATE_REGEX, (const char*)token, &matchlen);
    if (match > -1) {
        strptime((const char*)token, DATE_FMT, &t->created_date);
        token = strtok(NULL, sep);
    }

    char todo[MAX_LINE_LENGTH] = {0};
    int todo_idx = 0;
    int pri_match, pri_matchlen;
    int due_match, due_matchlen;

    // loop through remaining tokens
    while (token != NULL) {
        pri_match = re_match(PRIORITY_KV_REGEX, (const char*)token, &pri_matchlen);
        due_match = re_match(DUEDATE_REGEX, (const char*)token, &due_matchlen);

        if (pri_match > -1 && t->priority == NO_PRIORITY_CHARACTER) {
            t->priority = token[pri_match+pri_matchlen-1]; // last char is priority
        }
        else if (due_match > -1) {
            strptime((const char*)token, DUEDATE_FMT, &t->due_date);
        }
        else {
            int len = strlen(token);
            if (todo_idx+len >= MAX_LINE_LENGTH)
                len = MAX_LINE_LENGTH - todo_idx;
            strncpy(&todo[todo_idx], token, len);
            todo_idx = todo_idx + len;
            todo[todo_idx++] = ' '; // add 1 for space
        }

        // check projects
        match = re_match(PROJECT_REGEX, (const char*)token, &matchlen);
        if (match > -1 && n_projects < MAX_TAGS) {
            strncpy(t->projects[n_projects++], (const char*)token, CAP_LENGTH(strlen(token)));
        }

        // check contexts
        match = re_match(CONTEXT_REGEX, (const char*)token, &matchlen);
        if (match > -1 && n_contexts < MAX_TAGS) {
            strncpy(t->contexts[n_contexts++], (const char*)token, CAP_LENGTH(strlen(token)));
        }
        token = strtok(NULL, sep);
    }

    strncpy(t->todo, todo, MAX_LINE_LENGTH);
    return 0;
}


int rebuild_task(Task* t, char* o)
{
    int res;
    char buffer[MAX_LINE_LENGTH] = {0};
    int buf_idx = 0;

    if (t->finished == true) {
        buffer[buf_idx++] = 'x';
        buffer[buf_idx++] = ' ';

        if (t->finished_date.tm_year != 0) {
            res = strftime(&buffer[buf_idx], 11, DATE_FMT, &t->finished_date); // writes '\0' at the end
            if (res > 0) {
                buf_idx += res;
                buffer[buf_idx++] = ' ';
            }
        }
    }
    else if (t->priority != NO_PRIORITY_CHARACTER) {
        snprintf(&buffer[buf_idx], 5, "(%c) ", t->priority);
        buf_idx += 4;
    }

    if (t->created_date.tm_year != 0) {
        res = strftime(&buffer[buf_idx], 11, DATE_FMT, &t->created_date); // writes '\0' at the end
        if (res > 0) {
            buf_idx += res;
            buffer[buf_idx++] = ' ';
        }
    }

    strncpy(&buffer[buf_idx], t->todo, CAP_LENGTH(strlen(t->todo)));
    buf_idx += CAP_LENGTH(strlen(t->todo));

    if (t->due_date.tm_year != 0) {
        strncat(&buffer[buf_idx], "due:", 5);
        buf_idx += 4;
        res = strftime(&buffer[buf_idx], 11, DATE_FMT, &t->due_date); // writes '\0' at the end
        if (res > 0) {
            buf_idx += res;
            buffer[buf_idx++] = ' ';
        }
    }

    if (t->finished == true && t->priority != NO_PRIORITY_CHARACTER) {
        snprintf(&buffer[buf_idx], 7, "pri:%c ", t->priority);
        buf_idx += 6;
    }

    strncpy(o, buffer, MAX_LINE_LENGTH);
    return 0;
}

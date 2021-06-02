#include <stdio.h>

#include "task.h"


int main()
{

    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    FILE* fp = fopen("/home/shashankgopikrishna/Dropbox/todo.txt/todo.txt", "r");
    if (fp == NULL)
        return -1;

    while ((read = getline(&line, &len, fp)) != -1) {
        Task t;
        char o[MAX_LINE_LENGTH];
        parse_task(&t, line);
        rebuild_task(&t, o);
        printf("%d - %s\n", t.tid, o);

    }
    return 0;
}

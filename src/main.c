#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "todo.h"
#include "ui.h"
#include "ansictrl.h"


#define CONFIG_FILE "todo.conf"


// file reader
typedef void (*getline_cb_t)(void* data, char* line); // callback type for iterlines over a file

void iterlines(FILE* fp, getline_cb_t cb, void* data);
void read_conf(void* todos, char* line); // is of type getline_cb_t




// cli commands

typedef struct {
    char cmd_str[64];
    int len;
} cmd_str_t;

void cmd_add(cmd_str_t*, char);
void cmd_pop(cmd_str_t*);
void cmd_clear(cmd_str_t*);
int cmd_getint(cmd_str_t*);


typedef enum {
    NOOP = -1,
    WRITE,
    QUIT,
    WRITE_QUIT,
    SORT,
    FILTER,
} cmd_var;


struct cmd_lookup_t {
    char* cmd_str;
    cmd_var cmd;
};


static struct cmd_lookup_t cmd_lookup_table[] = {
    {"q", QUIT},
    {"quit", QUIT},
    {"w", WRITE},
    {"write", WRITE},
    {"wq", WRITE_QUIT},
    {"s", SORT},
    {"sort", SORT},
    {"f", FILTER},
    {"filter", FILTER},
};

#define NCMDS (sizeof(cmd_lookup_table)/sizeof(struct cmd_lookup_t))

cmd_var lookup_cmd(char* cmd_str);
void perform_sort_cmd(TodoUI* ui, char* cmd_str);


typedef int (*command_cb_t) (TodoUI*, cmd_str_t*);
int command_mode(TodoUI* ui, command_cb_t step_cb, command_cb_t final_cb);


// impl cmd

void cmd_add(cmd_str_t* m, char c)
{
	m->cmd_str[m->len] = c;
	m->cmd_str[++m->len] = '\0';
}

void cmd_pop(cmd_str_t* m)
{
	m->cmd_str[--m->len] = '\0';
}

void cmd_clear(cmd_str_t* m)
{
	m->cmd_str[0] = '\0';
	m->len = 0;
}

int cmd_getint(cmd_str_t* m)
{
	if (m->len == 0)
		return -1;

	for (int i=0; i<m->len; i++)
	{
		if (m->cmd_str[i] > '9' && m->cmd_str[i] < '0')
			return -1;
	}
	return atoi(m->cmd_str);
}


cmd_var lookup_cmd(char* cmd_str)
{
    char* tempstr = calloc(strlen(cmd_str)+1, sizeof(char));
    strncpy(tempstr, cmd_str, strlen(cmd_str)+1);
    char* token = strtok(tempstr, " ");

    cmd_var out = NOOP;
    for (int i=0; i < NCMDS; i++) {
        if (strcmp(cmd_lookup_table[i].cmd_str, token) == 0) {
            out = cmd_lookup_table[i].cmd;
        }
    }
    free(tempstr);
    return out;
}



// impl functionality

static void inner_sort_by_char(TodoUI* ui, char cmd_char)
{
    switch(cmd_char) {
        case 'f':
            todoslice_sort(ui->todos, FINISHED_DATE);
            todoui_draw(ui);
            break;

        case 'c':
            todoslice_sort(ui->todos, CREATED_DATE);
            todoui_draw(ui);
            break;

        case 'p':
            todoslice_sort(ui->todos, PRIORITY);
            todoui_draw(ui);
            break;

        case 'd':
            todoslice_sort(ui->todos, DUE_DATE);
            todoui_draw(ui);
            break;

        case 'i':
            todoslice_sort(ui->todos, ID);
            todoui_draw(ui);
            break;
    }
}

void perform_sort_cmd(TodoUI* ui, char* cmd_str)
{
    char* token = strtok(cmd_str, " "); // this is the actual sort command
    token = strtok(NULL, " "); // this will be the first argument if any. this is the only arg we care about
    if (token != NULL) {
        inner_sort_by_char(ui, token[0]);
        return;
    }

    int ch;
    bool done = false;
    while (!done) {
        printf(COLOR_YELLOW);
        printf("\nSort by:\nf: finished date\nc: created date\np: priority\nd: due date\ni: ID\nPress ESC key to exit sort mode.");
        printf(COLOR_RESET);
        todoui_reset_cursor(ui);

        ch = read_keypress();
        if (ch == ESCAPE) {
            todoui_draw(ui);
            done = true;
        }
        else {
            inner_sort_by_char(ui, ch);
        }
    }
}



void write_to_file_cmd(TodoArray* t, const char* filename)
{
    FILE* fp = fopen(filename, "w");
    if (fp == NULL)
        return;

    for (int i=0; i<t->n_todos; i++) {
        char buffer[MAX_LINE_LENGTH] = {0};
        todo_rebuild(&t->todos[i], buffer);
        fputs(buffer, fp);
        fputc('\n', fp);
    }

    fclose(fp);
}


int general_command_cb(TodoUI* ui, cmd_str_t* cmd)
{
    int ret = 0;
    if (cmd->len > 0) {
        switch(lookup_cmd(cmd->cmd_str)) {
            case WRITE:
                write_to_file_cmd(ui->todos, "test.txt");
                break;

            case QUIT:
                ret = 1;
                break;

            case WRITE_QUIT:
                write_to_file_cmd(ui->todos, "test.txt");
                ret = 1;
                break;

            case SORT:
                perform_sort_cmd(ui, cmd->cmd_str);
                break;

            default:
                break;
        }
    }
    return ret;
}



int search_command_cb(TodoUI* ui, cmd_str_t* cmd)
{
    bool should_reset = true;

    // move virtual cursor to top row to avoid going out of range
    todoui_vc_home(ui);
    if (cmd->len > 0) {
        todoslice_search(ui->todos, cmd->cmd_str);

        if (ui->todos->n_slice > 0) {
            todoui_draw(ui);
            should_reset = false;
        }
    }

    // only reset if slice is different from actual array because speeeeeed!!!
    if (should_reset && (ui->todos->n_todos-ui->todos->n_slice)!=0) {
        todoslice_create(ui->todos);
        todoui_draw(ui);
    }

    return 0;
}



int command_mode(TodoUI* ui, command_cb_t step_cb, command_cb_t final_cb)
{
    int ch;
    cmd_str_t cmd = {"", 0};
    bool done = false;

    while (!done) {
        ch = read_keypress();
        switch(ch) {
            case ENTER:
                done = true;
                break;

            case ESCAPE:
                cmd_clear(&cmd);
                done = true;
                break;

            case BACK_SPACE:
                putchar('\b');
                putchar(' ');
                putchar('\b');
                if (cmd.len>0)
                    cmd_pop(&cmd);
                else
                    done=true;
                break;

            case 1 ... 255: // ascii range
                putchar(ch);
                cmd_add(&cmd, ch);
                break;

            default:
                break;
        }
        if (step_cb)
            step_cb(ui, &cmd);
    }
    todoui_reset_cursor(ui);
    console_clear_line();

    int quit = 0;

    if (final_cb)
        quit = final_cb(ui, &cmd);

    return quit;
}



// file reader
void iterlines(FILE* fp, getline_cb_t cb, void* data)
{
#if PLATFORM_WIN == 1
	char line[1000];
	int size = 1000 * sizeof(char);
	while(fgets(line, size, fp) != NULL) {
		cb(data, line);
	}
#else
	char* line = (char*) malloc(sizeof(char) * 1000);
	size_t size = 1000 * sizeof(char);
	while(getline(&line, &size, fp) != EOF) {
		cb(data, line);
	}
	free(line);
#endif
}

void read_conf(void* todos, char* line)
{
    if (line[0]=='#') return; // commented

    int cpy_len;
    char filename[256];// = "todo.txt";

    if (line[strlen(line)-1] == '\n')
        cpy_len = strlen(line)-1;
    else
        cpy_len = strlen(line);

    strncpy(filename, line, cpy_len);
    filename[cpy_len] = '\0';

    printf("%s\n", filename);
    FILE* fp = fopen(filename, "r");
    if (fp == NULL)
        return;

    iterlines(fp, todoarray_add_cb, (TodoArray*)todos);
    fclose(fp);
}




int main (int argc, char* argv[])
{
    TodoArray todos = todoarray_init();

    {
        FILE* fp = fopen(CONFIG_FILE, "r");
        if (fp == NULL)
            goto error;

        iterlines(fp, read_conf, &todos);
        fclose(fp);
    }

    // default sort
    todoslice_create(&todos);

    TodoUI ui = todoui_init(&todos, argv[0], "todo.txt");
    todoui_draw(&ui);

    int ch, n;
    int cmd_res;
    cmd_str_t nav_cmd = {"", 0}; // navigation store for commands like 5j (navigate down 5 rows)

    bool done = false;
    while (!done) {
        ch = read_keypress();
        switch(ch) {

            case ENTER:
                todoui_draw(&ui);
                cmd_clear(&nav_cmd);
                break;

            case ESCAPE:
                todoui_reset_cursor(&ui);
                console_clear_line();
                cmd_clear(&nav_cmd);
                if (todos.n_todos!=todos.n_slice) {
                    todoslice_create(&todos);
                    todoui_draw(&ui);
                }
                // done = true;
                break;

            case 'j':
            case DOWN:
                n = cmd_getint(&nav_cmd);
                if (n < 0) n = 1;
                todoui_vc_down(&ui, n);
                cmd_clear(&nav_cmd);
                break;

            case 'k':
            case UP:
                n = cmd_getint(&nav_cmd);
                if (n < 0) n = 1;
                todoui_vc_up(&ui, n);
                cmd_clear(&nav_cmd);
                break;

            case '0' ... '9':
                cmd_add(&nav_cmd, ch);
                break;

            case ':':
                putchar(ch);
                cmd_res = command_mode(&ui, NULL, general_command_cb);
                if (cmd_res!=0)
                    done = true;
                break;

            case '/':
                putchar(ch);
                cmd_res = command_mode(&ui, NULL, search_command_cb);
                if (cmd_res!=0)
                    done = true;
                break;

            default:
                // putchar(ch);
                break;
        }
    }
    printf("\nExit!\n");
    todoarray_release(&todos);
    return 0;

error:
    printf("File not found!\n");
    return -1;
}

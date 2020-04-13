#ifndef DUMMYSHELL_H_
#define DUMMYSHELL_H_

#include <stdlib.h>
#include <stdbool.h>

#define BUFFER_SIZE 256
#define COMMAND_SIZE 8
#define COMMAND_BUFFER_SIZE 64
#define MAX_ARGC 16

typedef struct shell_builtin {
    char builtin_name[COMMAND_SIZE];
    void (*callback)(char*);
} shell_builtin_t;

void cd_handler(char*);
void exit_handler(char*);
void clear_handler(char*);

void init_shell_loop();

void prompt_command();
bool execute_command(char* buffer);
bool read_command(char** buffer);
bool execute_builtin(char* buffer);

void parse_arguments(char* buffer, char* argv[MAX_ARGC]);

#endif
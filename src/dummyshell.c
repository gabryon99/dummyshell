#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dummyshell.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <libgen.h>
#include <stdio_ext.h>


#define CHECK_MALLOC(ptr, message) \
    if (ptr == NULL) { \
        perror(message); \
        exit(EXIT_FAILURE); \
    } \

static shell_builtin_t SHELL_BUILTINS[] = {
    {"cd", cd_handler}, 
    {"exit", exit_handler},
    {"clear", clear_handler},
};

void cd_handler(char* input) {

    char* path = strstr(input, " ") + 1;
    char* token_state = NULL;
    char* tokenized_path = strtok_r(path, "\n", &token_state);

    fprintf(stdout, "path: %s\n", tokenized_path);

    if (chdir(tokenized_path) == -1) {
        perror("Error while changing directory");
    }

    return;
}

void clear_handler(char* input) {
    if (system("clear") == -1) {
        perror("Error while clearing terminal");
    }
    return;
}

void exit_handler(char* input) {
    exit(EXIT_SUCCESS);
}

void init_shell_loop() {

    char command_buffer[COMMAND_BUFFER_SIZE];
    char* buffer = command_buffer;

    while (true) {
        
        prompt_command();
        if (!read_command(&buffer)) continue;
        if (execute_builtin(buffer)) continue;

        if (!execute_command(buffer)) {
            fprintf(stderr, "dummyshell :: command not found!\n");
        }

    }

    return;
}

void prompt_command() {

    char current_dir[BUFFER_SIZE], hostname[BUFFER_SIZE];

    getcwd(current_dir, BUFFER_SIZE);
    gethostname(hostname, BUFFER_SIZE);

    char* login_name = getlogin();

    fprintf(stdout, "%s@%s:%s $ ", login_name, hostname, current_dir);
    return;
}

bool execute_command(char* buffer) {

    char* argv[MAX_ARGC];
    parse_arguments(buffer, argv);

    pid_t pid = fork();

    if (pid == 0) {

        int status = 0;

        if (strncmp(argv[0], "./", 2) == 0 || strncmp(argv[0], "/", 1) == 0) {
            status = execv(argv[0], argv);
        }
        else {
            status = execvp(argv[0], argv);
        }

        if (status == -1) exit(errno);
    }
    
    int child_status = 0;
    waitpid(pid, &child_status, 0);
    for (size_t i = 0; i < MAX_ARGC; i++) {
        if (argv[i] != NULL) free(argv[i]);
    }

    if (WEXITSTATUS(child_status) == EXIT_SUCCESS) {
        return true;
    }

    return false;
}

void parse_arguments(char* buffer, char* argv[MAX_ARGC]) {

    for (size_t i = 0; i < MAX_ARGC; i++) argv[i] = NULL;

    char* buffer_copy = malloc(sizeof(char) * strlen(buffer));
    CHECK_MALLOC(buffer_copy, "Error while allocating space for copy buffer");
    strncpy(buffer_copy, buffer, strlen(buffer));
    buffer_copy[strlen(buffer) - 1] = '\0';

    size_t i = 0;

    char* token_state = NULL;
    for (char* arg = strtok_r(buffer_copy, " \n", &token_state); arg != NULL; arg = strtok_r(NULL, " \n", &token_state), i++) {
        
        size_t arg_len = strlen(arg) + 1;
        char* command = malloc(arg_len * sizeof(char));
        CHECK_MALLOC(command, "Error while allocating memoery for argument");
        strncpy(command, arg, arg_len - 1);
        command[arg_len - 1] = '\0';
        argv[i] = command;
    }

    for (size_t j = i; j < MAX_ARGC; j++) argv[j] = NULL;
    
    free(buffer_copy);
    return;
}

bool read_command(char** buffer) {

    if (fgets(*buffer, COMMAND_BUFFER_SIZE, stdin) == NULL) {
        fprintf(stdout, "\n");
        clearerr(stdin);
        return false;
    }

    return true;
}

bool execute_builtin(char* buffer) {

    for (size_t i = 0; i < sizeof(SHELL_BUILTINS) / sizeof(shell_builtin_t); i++) {

        const char* builtin_name = SHELL_BUILTINS[i].builtin_name;

        if (strncmp(buffer, builtin_name, strlen(builtin_name)) == 0) {
            SHELL_BUILTINS[i].callback(buffer);
            return true;
        }
    }

    return false;
}
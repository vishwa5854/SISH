//
// Created by z on 12/17/22.
//

#ifndef SISH_DATA_STRUCTURES_H_
#define SISH_DATA_STRUCTURES_H_

#include <stdbool.h>
#include <limits.h>

// #ifndef LOGIN_NAME_MAX
// #define LOGIN_NAME_MAX 256
// #endif

#define MAX_COMMAND_SIZE 4096

#define PROGRAM_NAME "SISH"

typedef struct FLAGS_STRUCT {
    bool c;
    char command[MAX_COMMAND_SIZE + 1];
    bool x;
} FLAGS;

typedef struct command {
    char *command_str;
    struct command *next;
} TokenizedIndividualCommand;

typedef struct ParsedCommand {
    char *exec_name;
    char *original_command;
    char **args;
    /** Note that we could only have one input or output redirections */
    char *input_redirection;
    char *output_redirection;
    int append;
    int n_args;
    struct ParsedCommand *next;
} PCommand;

typedef struct MasterCommand {
    TokenizedIndividualCommand *head_command;
    TokenizedIndividualCommand *current_command;
    struct MasterCommand *next;
} MasterCommand;

#endif  // SISH_DATA_STRUCTURES_H

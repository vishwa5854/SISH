//
// Created by z on 12/17/22.
//
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "command-parser.h"
#include "data-structures.h"
#include "flags-parser.h"
#include "shell-builtins.h"
#include "util.h"

#ifndef ARG_MAX
#define ARG_MAX 8192
#endif

FLAGS flags;
MasterCommand *current_mc;
MasterCommand *head_mc;
PCommand *parsed_command;

int exit_status_of_last_command = 0;
int current_process_id = 0;

int main(int argc, char **argv) {
    sigset_t mask;
    (void)signal(SIGINT, SIG_IGN);
    (void)signal(SIGQUIT, SIG_IGN);
    (void)signal(SIGTSTP, SIG_IGN);

    if ((sigemptyset(&mask) == -1) || (sigaddset(&mask, SIGCHLD) == -1)) {
        exit(EXIT_FAILURE);
    }

    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        exit(EXIT_FAILURE);
    }

    set_env_shell();
    parse_flags(&flags, argc, argv);

    /** Execute only command given in -c */
    if (flags.c) {
        current_mc = (MasterCommand *)malloc(sizeof(MasterCommand));
        head_mc = current_mc;
        parse_and_exec(flags.command, current_mc, &flags);
        free(current_mc);
    } else {
        char input[ARG_MAX];

        while (1) {
            current_mc = (MasterCommand *)malloc(sizeof(MasterCommand));
            head_mc = current_mc;
            printf("SISH$ ");
            fgets(input, BUFSIZ, stdin);

            if ((strncmp(input, "exit", strlen("exit")) == 0) ||
                (strncmp(input, "exit\n", strlen("exit\n")) == 0) ||
                (strncmp(input, "exit\r\n", strlen("exit\r\n")) == 0)) {
                exit_sish();
                break;
            }

            if ((strncmp(input, "\n", strlen("\n")) == 0) ||
                (strncmp(input, "\r\n", strlen("\r\n")) == 0)) {
                continue;
            }

            parse_and_exec(input, current_mc, &flags);
            free(current_mc);
        }
    }

    return EXIT_SUCCESS;
}

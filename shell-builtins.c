//
// Created by z on 12/17/22.
//
#include "shell-builtins.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "data-structures.h"

char current_user[LOGIN_NAME_MAX];
int last_command_status;
int is_kid = 0;

void cd(char *path) {
    if (path == NULL) {
        if (getlogin_r(current_user, LOGIN_NAME_MAX) != 0) {
            (void)fprintf(stderr, "%s: cd: %s\n", PROGRAM_NAME, strerror(errno));
            last_command_status = errno;

            if (is_kid) {
                _exit(last_command_status);
            }
            return;
        }

        path = (char *)malloc(sizeof(char) * (strlen(current_user) + strlen("/home/")));
        (void)strncpy(path, "/home/", strlen("/home/"));
        (void)strncat(path, current_user, strlen(current_user));
    }

    if (chdir(path) < 0) {
        (void)fprintf(stderr, "%s: cd: %s\n", PROGRAM_NAME, strerror(errno));
        last_command_status = errno;

        if (is_kid) {
            _exit(last_command_status);
        }
        return;
        /** We should not exit because the shell should continue even when cd fails :) */
    }
    last_command_status = EXIT_SUCCESS;

    if (is_kid) {
        _exit(last_command_status);
    }
}

void echo(char *word) {
    if (strncmp(word, "$?", strlen(word)) == 0) {
        (void)printf("%d\n", last_command_status);
    } else if (strncmp(word, "$$", strlen(word)) == 0) {
        (void)printf("%d\n", getppid());
    } else {
        (void)printf("%s\n", word);
    }
    last_command_status = EXIT_SUCCESS;

    if (is_kid) {
        _exit(last_command_status);
    }
}

void exec_built_in(PCommand *head, int is_child) {
    is_kid = is_child;
    if (strncmp(head->exec_name, "cd", strlen("cd")) == 0) {
        /** No path is given */
        if (head->n_args == 1) {
            cd(NULL);
        } else if (head->n_args == 2) {
            cd(head->args[1]);
        } else if (head->n_args > 2) {
            /** Usage print */
            (void)fprintf(stderr, "%s: Usage cd [dir]\n", PROGRAM_NAME);
            last_command_status = EXIT_FAILURE;

            if (is_kid) {
                _exit(last_command_status);
            }
            return;
        }
    } else if (strncmp(head->exec_name, "echo", strlen("echo")) == 0) {
        if (head->n_args == 1) {
            (void)printf("\n");
        } else if (head->n_args == 2) {
            echo(head->args[1]);
        } else if (head->n_args > 2) {
            last_command_status = EXIT_FAILURE;
            (void)fprintf(stderr, "%s: Usage echo [word]\n", PROGRAM_NAME);

            if (is_kid) {
                _exit(last_command_status);
            }
            return;
        }
    }
}

void set_last_command_status(int status) { 
    last_command_status = status; 
}

void exit_sish() { 
    exit(last_command_status); 
}
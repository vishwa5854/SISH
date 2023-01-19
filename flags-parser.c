//
// Created by z on 12/17/22.
//
#include "flags-parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "shell-builtins.h"

void parse_flags(FLAGS *flags, int argc, char **argv) {
    bzero(flags->command, MAX_COMMAND_SIZE + 1);
    int option;

    while ((option = getopt(argc, argv, "c:x")) != -1) {
        switch (option) {
            case 'c':
                (void)strncpy(flags->command, optarg, strnlen(optarg, MAX_COMMAND_SIZE));
                flags->command[strnlen(optarg, MAX_COMMAND_SIZE)] = '\t';
                flags->command[strnlen(optarg, MAX_COMMAND_SIZE) + 1] = '\0';
                flags->c = true;
                break;
            case 'x':
                flags->x = true;
                break;
            default:
                (void)fprintf(stderr, "Usage: %s [ −x] [ −c command]\n", PROGRAM_NAME);
                exit(EXIT_FAILURE);
        }
    }
    argc -= optind;

    if (argc != 0) {
        (void)fprintf(stderr, "Usage: %s [ −x] [ −c command]", PROGRAM_NAME);
        set_last_command_status(EXIT_FAILURE);
        exit(EXIT_FAILURE);
    }
}
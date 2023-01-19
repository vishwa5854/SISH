//
// Created by z on 12/19/22.
//

#ifndef SISH_COMMAND_PARSER_H
#define SISH_COMMAND_PARSER_H

#include "data-structures.h"

int parse_one_command(char *original_command, TokenizedIndividualCommand *current);

void find_the_executable_and_args(TokenizedIndividualCommand *top, int n_args, PCommand *p_command);

void parse_and_exec(char *full_command, MasterCommand *current_mc, FLAGS *flags);

#endif  // SISH_COMMAND_PARSER_H

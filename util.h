//
// Created by z on 12/21/22.
//

#ifndef SISH_UTIL_H
#define SISH_UTIL_H

#include "data-structures.h"

int is_redirection(char *token);

int check_permissions(char *redirection_token, char *file_path);

int has_valid_redirection(TokenizedIndividualCommand *top);

int is_input_redirection(char *token);

int is_output_redirection(char *token);

int is_append_redirection(char *token);

int number_of_pipes(char *token);

void set_env_shell();

int is_built_in(char *command);

#endif  // SISH_UTIL_H

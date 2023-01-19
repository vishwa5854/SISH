//
// Created by z on 12/17/22.
//

#ifndef SISH_SHELL_BUILTINS_H
#define SISH_SHELL_BUILTINS_H

#include "data-structures.h"
void cd(char *path);

void exec_built_in(PCommand *head, int is_child);

void set_last_command_status(int status);

void exit_sish();

#endif  // SISH_SHELL_BUILTINS_H

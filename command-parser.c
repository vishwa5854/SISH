//
// Created by z on 12/19/22.
//
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "executor.h"
#include "shell-builtins.h"
#include "util.h"

/** TODO: Please add an empty space at the end of original_command to get all
 * tokens */
int parse_one_command(char *original_command, TokenizedIndividualCommand *current) {
    size_t n_original_command = strlen(original_command);
    size_t i = 0, word_length = 0, n_tokens = 0;
    char token[BUFSIZ];
    bzero(token, sizeof(token));
    int i_updated = 0;
    int out_redirection = 0;
    int in_redirection = 0;

    while (i < n_original_command) {
        if (isspace(original_command[i]) || (original_command[i] == '>') ||
            (original_command[i] == '<')) {
            /** Time to flush the token and there might be a white space in the
             * beginning */
            if (word_length > 0) {
                current->command_str = (char *)malloc(sizeof(char) * word_length);
                (void)strncpy(current->command_str, token, word_length);
                current->next =
                    (TokenizedIndividualCommand *)malloc(sizeof(TokenizedIndividualCommand));
                current = current->next;
                n_tokens++;
                bzero(token, sizeof(token));
            }

            if ((original_command[i] == '>') || (original_command[i] == '<')) {
                /** We already had an stdout redirection bruh */
                if (out_redirection && (original_command[i] == '>')) {
                    return -1;
                }

                /** We already had an stdin redirection bruh */
                if (in_redirection && (original_command[i] == '<')) {
                    return -1;
                }
                out_redirection = (original_command[i] == '>');
                in_redirection = (original_command[i] == '<');

                /** Append Condition */
                if ((original_command[i] == '>') && ((i + 1) < n_original_command) &&
                    (original_command[i + 1] == '>')) {
                    current->command_str = (char *)malloc(sizeof(char) * 2);
                    (void)strncpy(current->command_str, ">>", 3);
                    i += 2;
                    i_updated = 1;
                } else {
                    current->command_str = (char *)malloc(sizeof(char));
                    current->command_str[0] = original_command[i];
                }

                current->next =
                    (TokenizedIndividualCommand *)malloc(sizeof(TokenizedIndividualCommand));
                current = current->next;
                n_tokens++;
            }
            word_length = 0;
        } else {
            /** I am currently not going to accept individual token more than
             * BUFSIZ */
            if (word_length < BUFSIZ) {
                token[word_length++] = original_command[i];
            }
        }

        if (!i_updated) {
            i++;
        } else {
            i_updated = 0;
        }
    }

    return (int)n_tokens;
}

void find_the_executable_and_args(TokenizedIndividualCommand *top, int n_args,
                                  PCommand *p_command) {
    char *args[n_args];
    bzero(args, sizeof(args));
    int start = 0;
    char *exec = NULL;
    char *in_redirection = NULL, *out_redirection = NULL;
    size_t out_redirection_length = 0;
    size_t in_redirection_length = 0;

    /** If the user gives just ls, then we need to take 2 args one is ls and
     * other is NULL */
    p_command->args = (char **)malloc(sizeof(char *) * (n_args + 1));

    /** The first string without any redirections is the exec name */
    while ((top != NULL) && (start < n_args) && (top->command_str != NULL)) {
        if (is_redirection(top->command_str)) {
            if (top->next != NULL) {
                if (is_input_redirection(top->command_str)) {
                    in_redirection = top->next->command_str;
                    in_redirection_length = strlen(top->next->command_str);
                } else if (is_output_redirection(top->command_str)) {
                    if (is_append_redirection(top->command_str)) {
                        p_command->append = 1;
                    } else {
                        p_command->append = 0;
                    }
                    out_redirection = top->next->command_str;
                    /** Sometimes strings behave oddly, so keeping track of strlen */
                    out_redirection_length = strlen(top->next->command_str);
                }
                top = top->next->next;
            } else {
                top = NULL;
            }
        } else {
            /** This token doesn't belong to a redirection bruh */
            if (exec == NULL) {
                exec = top->command_str;
            }
            p_command->args[start++] = top->command_str;
            top = top->next;
        }
    }
    p_command->args[start] = NULL;
    p_command->n_args = start;

    if (exec != NULL) {
        p_command->exec_name = (char *)malloc(sizeof(char) * strlen(exec));
        (void)strncpy(p_command->exec_name, exec, strlen(exec));
    } else {
        p_command->exec_name = NULL;
    }

    if (in_redirection != NULL) {
        p_command->input_redirection = (char *)malloc(sizeof(char) * in_redirection_length);
        (void)strncpy(p_command->input_redirection, in_redirection, in_redirection_length);
    } else {
        p_command->input_redirection = NULL;
    }

    if (out_redirection != NULL) {
        p_command->output_redirection = (char *)malloc(sizeof(char) * out_redirection_length);
        (void)strncpy(p_command->output_redirection, out_redirection, out_redirection_length);
    } else {
        p_command->output_redirection = NULL;
    }
}

PCommand *current;
PCommand *head;

void parse_and_exec(char *full_command, MasterCommand *current_mc, FLAGS *flags) {
    int n_pipes = number_of_pipes(full_command);
    char *token = strtok(full_command, "|");
    int n_commands = 0;
    current = (PCommand *)malloc(sizeof(PCommand));
    head = current;

    while (token != NULL) {
        current_mc->current_command =
            (TokenizedIndividualCommand *)malloc(sizeof(TokenizedIndividualCommand));
        current_mc->head_command = current_mc->current_command;
        /** We have one individual command now :) */
        int n_tokens = parse_one_command(token, current_mc->current_command);

        if (n_tokens == -1) {
            set_last_command_status(EXIT_FAILURE);
            (void)fprintf(stderr, "%s: Invalid command: %s\n", PROGRAM_NAME, token);
            return;
        }
        TokenizedIndividualCommand *redirection_head = current_mc->head_command;

        if (has_valid_redirection(redirection_head) == EXIT_FAILURE) {
            set_last_command_status(EXIT_FAILURE);
            return;
        }

        TokenizedIndividualCommand *exec_head = current_mc->head_command;

        find_the_executable_and_args(exec_head, n_tokens, current);

        /**
         * We are allowing input redirection in the first command and out put
         * redirection in the last command but in between it doesn't really make
         * sense to allow redirection since it will mess up piping.
         * */
        if (n_pipes > 0) {
            if (n_commands == 0) {
                if (current->output_redirection != NULL) {
                    set_last_command_status(EXIT_FAILURE);
                    (void)fprintf(stderr, "%s: Invalid redirections and piping\n", PROGRAM_NAME);
                    return;
                }
            } else if (n_commands == n_pipes) {
                if (current->input_redirection != NULL) {
                    set_last_command_status(EXIT_FAILURE);
                    (void)fprintf(stderr, "%s: Invalid redirections and piping\n", PROGRAM_NAME);
                    return;
                }
            } else {
                /** This is in between pipes where input or output redirection doesn't make sense.
                 */
                if ((current->input_redirection != NULL) || (current->output_redirection != NULL)) {
                    set_last_command_status(EXIT_FAILURE);
                    (void)fprintf(stderr, "%s: Redirection in between pipes is not valid\n",
                                  PROGRAM_NAME);
                    return;
                }
            }
        }
        current->original_command = (char *)malloc(strlen(token));
        (void)strncpy(current->original_command, token, strlen(token));
        current->next = (PCommand *)malloc(sizeof(PCommand));
        current = current->next;
        n_commands += 1;
        token = strtok(NULL, "|");
    }

    executor(head, n_commands, flags);
    free(current);
}

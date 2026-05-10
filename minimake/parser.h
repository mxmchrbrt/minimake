#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>


struct parse_data {
    char *variable;
    char *rule;
    char **dependencies;
    char *variables_value;
    char **commands;
    char *comment;
    int line_pos;
};

struct data {
    struct parse_data *data;
    size_t size;
    size_t cap;
};

char **read_all_lines(char *filename);
char **clean_lines(char **lines);
void free_lines(char **lines);
void free_parse_data(struct parse_data *pd);
struct data parsing(char **lines);
void pretty_print(struct data final_data);

void print_lines(char **lines);
char *expand_variables_aux(char *line, struct data final_data);
char **expand_variables(char **lines, struct data final_data);
int execute_commands(struct data final_data, char **targets);
int already_visited(char *rule_name);
int run_command(char *command, const char *rule_name);
int execute_rule(struct data final_data, char *rule_name, const char *needed_by);

#endif

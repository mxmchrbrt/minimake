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

struct parse_data{
  char variable[100];
  char rule[100];
  char dependencies[100];
  char variables_value[100];
  char command[100];
  char comment[200];
  int line_pos;
};

struct data{
  struct parse_data *data;
  size_t size;
};

char **read_all_lines(char *filename);
char **clean_lines(char **lines);
void free_lines(char **lines);
struct data parsing(char **lines);
struct parse_data parsing_aux(char *lines);
void pretty_print(struct data final_data);
void execute_commands(struct data final_data);

#endif

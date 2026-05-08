#ifndef PARSER_H   /* Include guard */
#define PARSER_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

char **read_all_lines(char *filename);
char **clean_lines(char **lines);
void free_lines(char **lines);
void pretty_print(char **lines);
#endif

#ifndef BASICS_H   /* Include guard */
#define BASICS_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

struct target{
  char** target;
  int target_count;
};

int parse_options(int argc, char *argv[]);
#endif

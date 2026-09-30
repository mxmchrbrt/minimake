#ifndef BASICS_H   
#define BASICS_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "parser.h"

int parse_options(int argc, char *argv[]);

#endif

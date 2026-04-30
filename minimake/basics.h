#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>


enum definition{
  ERROR_DEF, RULE_DEF, VAR_DEF, NO_DEF
};

int which_definition(char *buffer);

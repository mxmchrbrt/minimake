#include "basics.h"
/*
// Tell if an arg is in the buffer + tell the position if present else return -1

int arg_pos(char *buffer, char arg){
  if (!buffer || !arg){
    return -1;
    }
  int pos;
  int found = 0;
  for (int i = 0; buffer[i] != '\0';i++){
    if (buffer[i] == arg){
      found = 1;
      break; // It is a rule definition
    }
    pos++;
  }
  if (found) return pos;
  else{
    return -1; // Arg not found in the buffer
  } 
}
*/

// Instead I will use strchr and strstr

int which_definition(char *buffer){
  if (!buffer){
    return ERROR_DEF; // An error occured
  }
  for (int i = 0; buffer[i] != '\0';i++){
    if (buffer[i] == ':'){
      return RULE_DEF; // It is a rule definition
    }
    if (buffer[i] == '='){
      return VAR_DEF; // It is a variable definition
    }
  }
  return NO_DEF; // No definition was found
}

// TO-DO
// -Create a detailled list of all you have to do.

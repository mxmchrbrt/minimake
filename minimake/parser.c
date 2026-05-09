#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

char **read_all_lines(char *filename){
    if (!filename) return NULL;
    FILE *stream = fopen(filename,"r");
    if (!stream) return NULL;

    size_t cap = 16;
    size_t cnt = 0;
    char **lines = calloc(cap,sizeof(char*));
    if (!lines) {
        fclose(stream);
        return NULL;
    }
    char *buf = NULL;
    size_t n = 0;
    ssize_t len;
    while ((len = getline(&buf, &n, stream)) != -1) {
        char *s = strdup(buf);
        if (!s) {
            free(buf);
            for (size_t k = 0; k < cnt; ++k) free(lines[k]);
            free(lines);
            fclose(stream);
            return NULL;
        }
        if (cnt + 1 >= cap) {
            size_t newcap = cap * 2;
            char **tmp = realloc(lines, newcap * sizeof(char*));
            if (!tmp) {
                free(buf);
                for (size_t k = 0; k < cnt; ++k) free(lines[k]);
                free(lines);
                fclose(stream);
                return NULL;
            }

            lines = tmp;
            cap = newcap;
        }
        lines[cnt] = s;
        cnt++;
    }
    free(buf);
    lines[cnt] = NULL;
    char **tmp = realloc(lines, (cnt + 1) * sizeof(char*));
    if (tmp) lines = tmp;
    fclose(stream);
    return lines;
}

void free_lines(char **lines){
    if (!lines) return;
    for (size_t i = 0; lines[i] != NULL; ++i) free(lines[i]);
    free(lines);
}

struct parse_data parsing_aux(char *lines){
  struct parse_data data;
  strcpy(data.variable,"");
  strcpy(data.rule,"");
  strcpy(data.dependencies,"");
  strcpy(data.variables_value,"");
  strcpy(data.command,"");
  strcpy(data.comment,"");
  if (!lines) return data;

  // comment
  char *p = strchr(lines, '#');
  if (p) {
    char *tmp = strdup(p + 1);
    if (tmp) {
      size_t len = strlen(tmp);
      if (len > 0 && tmp[len-1] == '\n') tmp[len-1] = '\0';
      strncpy(data.comment, tmp, sizeof(data.comment)-1);
      data.comment[sizeof(data.comment)-1] = '\0';
      free(tmp);
    }
  }

   //command
  char *p2 = strchr(lines, '\t');
  if (p2) {
    char *tmp = strdup(p2 + 1);
    if (tmp) {
      size_t len = strlen(tmp);
      if (len > 0 && tmp[len-1] == '\n') tmp[len-1] = '\0';
      strncpy(data.command, tmp, sizeof(data.command)-1);
      data.command[sizeof(data.command)-1] = '\0';
      free(tmp);
    }
  } 
  //definition
  char *p3 = strchr(lines, '=');
  if (p3) {
    char *tmp = strdup(p3 + 2);
    if (tmp) {
      size_t len = strlen(tmp);
      if (len > 0 && tmp[len-1] == '\n') tmp[len-1] = '\0';
      strncpy(data.variables_value, tmp, sizeof(data.variables_value)-1);
      data.command[sizeof(data.variables_value)-1] = '\0';
      free(tmp);
    }
  }  
  for (size_t i = 0; lines[i] != '\0'; i++){
   //variable name
   if (strchr(lines,'=') != NULL){
    sscanf(lines,"%[a-zA-Z0-9]=",data.variable); 
   }

   //rule
   if (strchr(lines,':') != NULL){
    sscanf(lines,"%[a-zA-Z0-9]:%[a-zA-Z0-9 ]",data.rule,data.dependencies); 
   } 
  }
  return data;
}

struct data parsing(char **lines){
  struct data final_data;
  final_data.size = 0;
  if (!lines) return final_data;

  struct parse_data *data = calloc(100,sizeof(struct parse_data)); 
  final_data.data = data;
  final_data.size = 0;
  for (size_t i = 0; lines[i] != NULL; i++){
    data[i] = parsing_aux(lines[i]);   
    data[i].line_pos = i+1;
    final_data.size++;
    // To check the parser get correct datas
    //printf("\nLines %d\n",data[i].line_pos);
    //printf("variable = %s\nrule = %s\ndependencies = %s\nvariables_value = %s\ncommand = %s\ncomment = %s\n", data[i].variable,data[i].rule,data[i].dependencies,data[i].variables_value,data[i].command,data[i].comment);
  }
  return final_data;
}

char **clean_lines(char **lines){ //remove spaces except those after ':' or after a '\t' or after '=' (keep dependency / command / assignment spacing)
  if (!lines) return NULL;
  for (size_t i = 0; lines[i] != NULL; ++i) {
    char *src = lines[i];
    char *dst = lines[i];
    size_t k = 0;
    int after_colon = 0;
    int after_tab = 0;
    int after_eq = 0;
    for (size_t j = 0; src[j] != '\0'; ++j) {
      if (src[j] == ':') {
        after_colon = 1;
        dst[k++] = src[j];
      } else if (src[j] == '\t') {
        after_tab = 1;
        dst[k++] = src[j];
      } else if (src[j] == '=') {
        after_eq = 1;
        dst[k++] = src[j];
      } else if (src[j] == ' ') {
        if (after_colon || after_tab || after_eq) dst[k++] = src[j];
      } else {
        dst[k++] = src[j];
      }
    }
    dst[k] = '\0';
  }
  return lines;
}

void print_lines(char **lines){
  if (!lines) return;
  for (size_t i = 0; lines[i] != NULL; i++) {
    char *cur_line = lines[i];
    printf("%s",cur_line);
  }
  return;
}

void pretty_print(struct data final_data){
  int variables = 0;
  int rules = 0;
  for (size_t i = 0; i < final_data.size; i++){
    if (final_data.data[i].variable[0] != '\0'){
      if (!variables){
        printf("# variables\n");
        variables = 1;
      }
      printf("'%s' = '%s'\n",final_data.data[i].variable,final_data.data[i].variables_value);
    }

    if (final_data.data[i].rule[0] != '\0'){
      if (!rules){
        printf("\n# rules\n");
        rules = 1;
      }
      if (final_data.data[i].dependencies[0] != '\0'){
        printf("(%s): %s\n",final_data.data[i].rule,final_data.data[i].dependencies);
      }
      else{
        printf("(%s):\n",final_data.data[i].rule);
      }
    }

    if (final_data.data[i].command[0] != '\0'){
      printf("\t'%s'\n",final_data.data[i].command);
    }
  }
}

void execute_commands(struct data final_data){
  if (!final_data.data) return;
  for (size_t i = 0; i < final_data.size; i++){
    const char *cmd = final_data.data[i].command;
    if (!cmd || cmd[0] == '\0') continue;
    printf("%s\n",final_data.data[i].command);
    fflush(stdout);
    pid_t pid = fork();
    if (pid == 0) {
      /* child: run command in a shell */
      execlp("sh", "sh", "-c", cmd, (char *)NULL);
      perror("execlp");
      _exit(127);
    } else if (pid > 0) {
      int status;
      if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
      } else {
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
          fprintf(stderr, "minimake: command failed (exit %d): %s\n", WEXITSTATUS(status), cmd);
        }
      }
    } else {
      perror("fork");
    }
  }
}

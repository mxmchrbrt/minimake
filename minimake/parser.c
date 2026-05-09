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
        //free(s);
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
  for (size_t i = 0; lines[i] != '\0'; i++){
   //definition
   if (strchr(lines,'=') != NULL){
    //sscanf(lines,"%s=%s"); 
   }

   //rule
   if (strchr(lines,':') != NULL){
   
   }

   //comment
   if (strchr(lines,'#') != NULL){
   
   }
  }
  return data;
}

void parsing(char **lines){
  if (!lines) return;
  struct parse_data *data = calloc(100,sizeof(struct parse_data)); 
  for (size_t i = 0; lines[i] != NULL; i++){
    data[i] = parsing_aux(lines[i]);   
    printf("\nLines %d\n",i);
    printf("variable = %s\nrule = %s\ndependencies = %s\nvariables_value = %s\ncommand = %s\ncomment = %s\n", data[i].variable,data[i].rule,data[i].dependencies,data[i].variables_value,data[i].command,data[i].comment);
  }
  free(data);
  return;
}

char **clean_lines(char **lines){ //remove spaces except those after ':' (keep dependency spacing)
  if (!lines) return NULL;
  for (size_t i = 0; lines[i] != NULL; ++i) {
    char *src = lines[i];
    char *dst = lines[i];
    size_t k = 0;
    int after_colon = 0;
    for (size_t j = 0; src[j] != '\0'; ++j) {
      if (src[j] == ':') {
        after_colon = 1;
        dst[k++] = src[j];
      }
      else if (src[j] == ' ') {
        if (after_colon) dst[k++] = src[j];
      }
      else {
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

int main(){
  char **lines = read_all_lines("txt");
  clean_lines(lines);
  //print_lines(lines);
  parsing(lines);
  free_lines(lines);
  return 0;
}

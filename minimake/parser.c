#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
                free(s);
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

void pretty_print(char **lines){ 
  if (!lines) return;
  int def_found = 0;
  int var_found = 0;
  for (size_t i = 0; lines[i] != NULL; i++){
    if ((strchr(lines[i],'=') != NULL) && !var_found){
      printf("# variables\n");
      var_found = 1; 
    }
    if ((strchr(lines[i],':') != NULL) && !def_found){
      printf("# rules\n");
      def_found = 1; 
    }
    
    printf("%s",(lines[i]));
  }
  return;
}

char **clean_lines(char **lines){
  if (!lines) return NULL;
  return lines;
}

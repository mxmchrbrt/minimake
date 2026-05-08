#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *remove_space(char *str){
    if (!str) return NULL;
    int i = 0, j = 0;
    while (str[i]) {
        if (str[i] != ' ') {
            str[j++] = str[i++];
          }
        else {
            if ((i > 0 && str[i-1] == '=') || (str[i+1] != '\0' && str[i+1] == '=')) {
                str[j++] = ' ';
            }
            i++;
        }
    }
    str[j] = '\0';
    return str;
}

char **remove_space_array(char **arr){
    if (!arr) return NULL;
    for (int i = 0; arr[i] != NULL; ++i) {
        remove_space(arr[i]);
    }
    return arr;
}

char **read_all_lines(FILE *stream){
    if (!stream) return NULL;
    size_t cap = 16;
    size_t cnt = 0;
    char **lines = malloc(cap * sizeof(char*));
    if (!lines) return NULL;
    char *buf = NULL;
    size_t n = 0;
    ssize_t len;
    while ((len = getline(&buf, &n, stream)) != -1) {
        char *s = strdup(buf);
        if (!s) {
            free(buf);
            for (size_t k = 0; k < cnt; ++k) free(lines[k]);
            free(lines);
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
                return NULL;
            }
            lines = tmp;
            cap = newcap;
        }
        lines[cnt++] = s;
    }
    free(buf);
    lines[cnt] = NULL;
    char **tmp = realloc(lines, (cnt + 1) * sizeof(char*));
    if (tmp) lines = tmp;
    return lines;
}

void free_lines(char **lines){
    if (!lines) return;
    for (size_t i = 0; lines[i] != NULL; ++i) free(lines[i]);
    free(lines);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *remove_space(char *str){ // remove all space but the one before and after '='
    if (!str) return NULL;
    int i = 0, j = 0;
    while (str[i]) {
        if (str[i] != ' ') {
            str[j++] = str[i++];
        } else {
            /* keep space only if it's immediately before or after '=' in the original string */
            if ((i > 0 && str[i-1] == '=') || (str[i+1] != '\0' && str[i+1] == '=')) {
                str[j++] = ' ';
            }
            i++;
        }
    }
    str[j] = '\0';
    return str;
}

/* Apply remove_space to each non-NULL element of a NULL-terminated array of strings.
   Returns the same array pointer. */
char **remove_space_array(char **arr){
    if (!arr) return NULL;
    for (int i = 0; arr[i] != NULL; ++i) {
        remove_space(arr[i]);
    }
    return arr;
}

/* Pretty-print lines obtained from getline: clean each line using remove_space_array and print.
   This strips trailing newlines first to avoid double newlines when printing. */
void pretty_print(char **lines){
    if (!lines) return;
    for (int i = 0; lines[i] != NULL; ++i) {
        size_t len = strlen(lines[i]);
        if (len > 0 && lines[i][len-1] == '\n') {
            lines[i][len-1] = '\0';
        }
    }
    /* clean all lines in-place */
    remove_space_array(lines);
    for (int i = 0; lines[i] != NULL; ++i) {
        printf("%s\n", lines[i]);
    }
}

/* Read all lines from a FILE* using getline and return a NULL-terminated array of malloc'ed strings.
   Caller must free the returned array and its strings (use free_lines). */
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
    /* shrink to fit */
    char **tmp = realloc(lines, (cnt + 1) * sizeof(char*));
    if (tmp) lines = tmp;
    return lines;
}

/* Free array returned by read_all_lines */
void free_lines(char **lines){
    if (!lines) return;
    for (size_t i = 0; lines[i] != NULL; ++i) free(lines[i]);
    free(lines);
}

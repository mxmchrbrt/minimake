#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

char **read_all_lines(char *filename) {
    if (!filename) return NULL;
    FILE *stream = fopen(filename, "r");
    if (!stream) return NULL;

    size_t cap = 16, cnt = 0;
    char **lines = calloc(cap, sizeof(char *));
    if (!lines) { fclose(stream); return NULL; }

    char *buf = NULL;
    size_t n = 0;
    ssize_t len;
    while ((len = getline(&buf, &n, stream)) != -1) {
        char *s = strdup(buf);
        if (!s) {
            free(buf);
            for (size_t k = 0; k < cnt; k++) free(lines[k]);
            free(lines); fclose(stream); return NULL;
        }
        if (cnt + 1 >= cap) {
            size_t nc = cap * 2;
            char **tmp = realloc(lines, nc * sizeof(char *));
            if (!tmp) {
                free(s); free(buf);
                for (size_t k = 0; k < cnt; k++) free(lines[k]);
                free(lines); fclose(stream); return NULL;
            }
            lines = tmp; cap = nc;
        }
        lines[cnt++] = s;
    }
    free(buf);
    lines[cnt] = NULL;
    char **tmp = realloc(lines, (cnt + 1) * sizeof(char *));
    if (tmp) lines = tmp;
    fclose(stream);
    return lines;
}

void free_lines(char **lines) {
    if (!lines) return;
    for (size_t i = 0; lines[i]; i++) { free(lines[i]); lines[i] = NULL; }
    free(lines);
}

char **split(char *str) {
    if (!str) return NULL;
    size_t count = 1;
    for (size_t i = 0; str[i]; i++)
        if (str[i] == ' ') count++;
    char **result = calloc(count + 1, sizeof(char *));
    if (!result) return NULL;
    size_t idx = 0;
    char *start = str;
    for (size_t i = 0; ; i++) {
        if (str[i] == ' ' || str[i] == '\0') {
            result[idx++] = strndup(start, &str[i] - start);
            start = &str[i] + 1;
            if (str[i] == '\0') break;
        }
    }
    result[idx] = NULL;
    return result;
}

void free_parse_data(struct parse_data *pd) {
    if (!pd) return;
    free(pd->variable);      pd->variable = NULL;
    free(pd->rule);          pd->rule = NULL;
    free(pd->variables_value); pd->variables_value = NULL;
    free(pd->comment);       pd->comment = NULL;
    free_lines(pd->dependencies);
    free_lines(pd->commands);
}

char **clean_lines(char **lines) {
    if (!lines) return NULL;
    for (size_t i = 0; lines[i]; i++) {
        char *src = lines[i];

        if (src[0] == '\t') continue;

        char *dst = lines[i];
        size_t k = 0;
        for (size_t j = 0; src[j]; j++) {
            dst[k++] = src[j];
        }
        dst[k] = '\0';
    }
    return lines;
}

void print_lines(char **lines) {
    if (!lines) return;
    for (size_t i = 0; lines[i]; i++) printf("%s", lines[i]);
}


static void strip_nl(char *s) {
    if (!s) return;
    size_t l = strlen(s);
    if (l > 0 && s[l-1] == '\n') s[l-1] = '\0';
}


static char **arr_append(char **arr, size_t *cnt, char *val) {
    char **tmp = realloc(arr, (*cnt + 2) * sizeof(char *));
    if (!tmp) { free(val); return arr; }
    tmp[*cnt] = val;
    (*cnt)++;
    tmp[*cnt] = NULL;
    return tmp;
}

struct data parsing(char **lines) {
    struct data final_data;
    final_data.size = 0;
    final_data.cap  = 0;
    final_data.data = NULL;
    if (!lines) return final_data;

    size_t cap = 64;
    struct parse_data *arr = calloc(cap, sizeof(struct parse_data));
    if (!arr) return final_data;
    final_data.data = arr;
    final_data.cap  = cap;

    size_t i = 0;
    while (lines[i]) {
        char *line = lines[i];


        if (final_data.size >= final_data.cap) {
            size_t nc = final_data.cap * 2;
            struct parse_data *tmp = realloc(final_data.data, nc * sizeof(struct parse_data));
            if (!tmp) break;
            final_data.data = tmp;
            final_data.cap  = nc;
        }


        if (line[0] == '\t') { i++; continue; }

        struct parse_data data;
        memset(&data, 0, sizeof(data));
        data.variable      = strdup("");
        data.rule          = strdup("");
        data.variables_value = strdup("");
        data.comment       = strdup("");
        data.dependencies  = NULL;
        data.commands      = NULL;
        data.line_pos      = (int)(i + 1);


        char *trimmed = line;
        while (*trimmed == ' ' || *trimmed == '\t') trimmed++;
        if (*trimmed == '\0' || *trimmed == '\n') {
            free_parse_data(&data);
            i++;
            continue;
        }


        char *cp = strchr(line, '#');
        if (cp) {
            free(data.comment);
            data.comment = strdup(cp + 1);
            strip_nl(data.comment);
        }


        char *colon = strchr(line, ':');
        char *eq    = strchr(line, '=');

        int is_rule = 0, is_var = 0;
        if (colon && (!eq || colon < eq)) is_rule = 1;
        else if (eq)                       is_var  = 1;

        if (is_var) {

            size_t name_len = eq - line;
            char *name_buf = strndup(line, name_len);

            size_t nl = strlen(name_buf);
            while (nl > 0 && (name_buf[nl-1] == ' ' || name_buf[nl-1] == '\t')) nl--;
            name_buf[nl] = '\0';

            char *name_start = name_buf;
            while (*name_start == ' ' || *name_start == '\t') name_start++;
            if (name_start != name_buf) memmove(name_buf, name_start, strlen(name_start) + 1);
            free(data.variable);
            data.variable = name_buf;


            char *val_start = eq + 1;
            while (*val_start == ' ' || *val_start == '\t') val_start++;
            free(data.variables_value);
            data.variables_value = strdup(val_start);
            strip_nl(data.variables_value);
        }

        if (is_rule) {

            size_t rule_len = colon - line;
            free(data.rule);
            data.rule = strndup(line, rule_len);
            {
                size_t rlen = strlen(data.rule);
                while (rlen > 0 && (data.rule[rlen-1] == ' ' || data.rule[rlen-1] == '\t'))
                    data.rule[--rlen] = '\0';
                char *rstart = data.rule;
                while (*rstart == ' ' || *rstart == '\t') rstart++;
                if (rstart != data.rule) memmove(data.rule, rstart, strlen(rstart) + 1);
            }


            char *deps_str = colon + 1;

            char *deps_copy = strdup(deps_str);
            strip_nl(deps_copy);
            char *hash = strchr(deps_copy, '#');
            if (hash) *hash = '\0';

            char *ds = deps_copy;
            while (*ds == ' ' || *ds == '\t') ds++;

            if (*ds != '\0') {
                data.dependencies = split(ds);

                if (data.dependencies) {
                    size_t wi = 0;
                    for (size_t ri = 0; data.dependencies[ri]; ri++) {
                        if (data.dependencies[ri][0] != '\0')
                            data.dependencies[wi++] = data.dependencies[ri];
                        else
                            free(data.dependencies[ri]);
                    }
                    data.dependencies[wi] = NULL;
                    if (wi == 0) {
                        free(data.dependencies);
                        data.dependencies = NULL;
                    }
                }
            }
            free(deps_copy);


            size_t cnt_cmds = 0;
            size_t k = i + 1;
            while (lines[k]) {
                char *cl = lines[k];
                if (cl[0] == '\t') {

                    char *cmd_line = cl + 1;
                    char *cmd = strdup(cmd_line);
                    strip_nl(cmd);
                    data.commands = arr_append(data.commands, &cnt_cmds, cmd);
                    k++;
                } else {

                    char *tr = cl;
                    while (*tr == ' ' || *tr == '\t') tr++;
                    if (*tr == '\0' || *tr == '\n' || *tr == '#') {
                        k++;
                        continue;
                    }

                    break;
                }
            }
        }

        final_data.data[final_data.size] = data;
        final_data.size++;
        i++;
    }
    return final_data;
}


void pretty_print(struct data fd) {
    int printed_var_header = 0;
    int printed_rule_header = 0;

    int has_vars = 0;
    for (size_t i = 0; i < fd.size; i++) {
        if (fd.data[i].variable && fd.data[i].variable[0] != '\0') {
            has_vars = 1;
            break;
        }
    }

    if (has_vars) {
        printf("# variables\n");
        printed_var_header = 1;
        for (size_t i = 0; i < fd.size; i++) {
            if (fd.data[i].variable && fd.data[i].variable[0] != '\0') {
                printf("'%s' = '%s'\n",
                       fd.data[i].variable,
                       fd.data[i].variables_value ? fd.data[i].variables_value : "");
            }
        }
    }

    if (printed_var_header) printf("\n");

    for (size_t i = 0; i < fd.size; i++) {
        if (fd.data[i].rule && fd.data[i].rule[0] != '\0') {
            if (!printed_rule_header) {
                printf("# rules\n");
                printed_rule_header = 1;
            }
            if (fd.data[i].dependencies) {
                printf("(%s):", fd.data[i].rule);
                for (size_t j = 0; fd.data[i].dependencies[j]; j++) {
                    if (fd.data[i].dependencies[j][0] != '\0')
                        printf(" [%s]", fd.data[i].dependencies[j]);
                }
                printf("\n");
            } else {
                printf("(%s):\n", fd.data[i].rule);
            }
            if (fd.data[i].commands) {
                for (size_t j = 0; fd.data[i].commands[j]; j++)
                    printf("\t'%s'\n", fd.data[i].commands[j]);
            }
        }
    }
    if (!printed_rule_header) printf("# rules\n");
}


char *expand_variables_aux(char *line, struct data final_data) {
    if (!line) return NULL;
    size_t in_len = strlen(line);
    size_t cap = in_len + 1;
    char *out = calloc(cap, 1);
    if (!out) return NULL;
    size_t pos = 0;

    for (size_t i = 0; line[i]; i++) {
        if (line[i] == '$' && line[i+1] == '{') {
            size_t j = i + 2;
            char name[256];
            size_t k = 0;
            while (line[j] && line[j] != '}' && k + 1 < sizeof(name))
                name[k++] = line[j++];
            name[k] = '\0';

            if (line[j] == '}' && k > 0) {
                const char *val = NULL;
                for (size_t z = 0; z < final_data.size; z++) {
                    if (final_data.data[z].variable &&
                        strcmp(name, final_data.data[z].variable) == 0) {
                        val = final_data.data[z].variables_value
                              ? final_data.data[z].variables_value : "";
                        break;
                    }
                }
                if (val == NULL) {
                } else {
                    size_t vlen = strlen(val);
                    if (pos + vlen + 1 > cap) {
                        size_t nc = cap ? cap : 1;
                        while (pos + vlen + 1 > nc) nc *= 2;
                        char *tmp = realloc(out, nc);
                        if (!tmp) { free(out); return NULL; }
                        out = tmp; cap = nc;
                    }
                    memcpy(out + pos, val, vlen);
                    pos += vlen;
                }
                i = j;
                continue;
            }

        }
        if (pos + 2 > cap) {
            size_t nc = cap ? cap * 2 : 64;
            char *tmp = realloc(out, nc);
            if (!tmp) { free(out); return NULL; }
            out = tmp; cap = nc;
        }
        out[pos++] = line[i];
    }
    out[pos] = '\0';
    return out;
}

char **expand_variables(char **lines, struct data final_data) {
    if (!lines) return NULL;
    for (size_t i = 0; lines[i]; i++) {

        if (lines[i][0] == '\t') continue;
        char *old = lines[i];
        char *new = expand_variables_aux(old, final_data);
        if (new) { free(old); lines[i] = new; }
    }
    return lines;
}

struct parse_data *find_rule(struct data final_data, char *name) {
    if (!name || name[0] == '\0') return NULL;
    for (size_t i = 0; i < final_data.size; i++) {
        if (!final_data.data[i].rule || final_data.data[i].rule[0] == '\0')
            continue;
        if (strcmp(final_data.data[i].rule, name) == 0)
            return &final_data.data[i];
    }
    return NULL;
}

int run_command(char *command, const char *rule_name) {
    printf("%s\n", command);
    fflush(stdout);

    pid_t pid = fork();
    if (pid == 0) {
        char *args[] = { "/bin/sh", "-c", command, NULL };
        execv("/bin/sh", args);
        perror("execv");
        exit(127);
    } else if (pid > 0) {
        int status;
        if (waitpid(pid, &status, 0) == -1) { perror("waitpid"); return 1; }
        int exit_code = 0;
        if (WIFEXITED(status))   exit_code = WEXITSTATUS(status);
        else if (WIFSIGNALED(status)) exit_code = 128 + WTERMSIG(status);
        if (exit_code != 0) {
            printf("minimake: *** [%s] Error %d\n",
                   rule_name ? rule_name : "unknown", exit_code);
            fflush(stdout);
            return exit_code;
        }
        return 0;
    } else {
        perror("fork");
        return 1;
    }
}

static char *visited[1024] = {NULL};
static size_t visited_count = 0;

int already_visited(char *rule_name) {
    for (size_t i = 0; i < visited_count; i++)
        if (strcmp(visited[i], rule_name) == 0) return 1;
    return 0;
}

int execute_rule(struct data final_data, char *rule_name, const char *needed_by) {
    struct parse_data *rule = find_rule(final_data, rule_name);
    if (!rule) {
        struct stat st;
        if (stat(rule_name, &st) == 0)
            return 0;
        if (needed_by)
            printf("minimake: *** No rule to make target '%s', needed by '%s'.  Stop.\n",
                   rule_name, needed_by);
        else
            printf("minimake: *** No rule to make target '%s'.  Stop.\n", rule_name);
        fflush(stdout);
        return 2;
    }
    if (already_visited(rule->rule)) return 0;
    visited[visited_count++] = rule->rule;

    if (rule->dependencies) {
        for (size_t j = 0; rule->dependencies[j]; j++) {
            if (rule->dependencies[j][0] != '\0') {
                int r = execute_rule(final_data, rule->dependencies[j], rule->rule);
                if (r != 0) return r;
            }
        }
    }

    if (rule->commands) {
        for (size_t j = 0; rule->commands[j]; j++) {
            char *cmd = expand_variables_aux(rule->commands[j], final_data);
            if (!cmd) cmd = strdup(rule->commands[j]);
            int r = run_command(cmd, rule->rule);
            free(cmd);
            if (r != 0) return r;
        }
    }
    return 0;
}

int execute_commands(struct data final_data, char **targets) {
    if (!final_data.data) {
        printf("minimake: *** No targets.  Stop.\n");
        return 2;
    }
    visited_count = 0;

    if (targets && targets[0]) {
        int ret = 0;
        for (size_t t = 0; targets[t]; t++) {
            int r = execute_rule(final_data, targets[t], NULL);
            if (r != 0) ret = 2;
        }
        return ret;
    }

    for (size_t i = 0; i < final_data.size; i++) {
        if (final_data.data[i].rule && final_data.data[i].rule[0] != '\0') {
            int r = execute_rule(final_data, final_data.data[i].rule, NULL);
            return r != 0 ? 2 : 0;
        }
    }

    printf("minimake: *** No targets.  Stop.\n");
    return 2;
}

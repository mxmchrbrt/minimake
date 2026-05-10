#include "basics.h"

static void expand_struct(struct data *fd) {
    for (size_t i = 0; i < fd->size; i++) {

        if (fd->data[i].variables_value) {
            char *e = expand_variables_aux(fd->data[i].variables_value, *fd);
            if (e) { free(fd->data[i].variables_value); fd->data[i].variables_value = e; }
        }

        if (fd->data[i].commands) {
            for (size_t j = 0; fd->data[i].commands[j]; j++) {
                char *e = expand_variables_aux(fd->data[i].commands[j], *fd);
                if (e) { free(fd->data[i].commands[j]); fd->data[i].commands[j] = e; }
            }
        }

        if (fd->data[i].rule && fd->data[i].rule[0] != '\0') {
            char *e = expand_variables_aux(fd->data[i].rule, *fd);
            if (e) { free(fd->data[i].rule); fd->data[i].rule = e; }
        }

        if (fd->data[i].dependencies) {
            for (size_t j = 0; fd->data[i].dependencies[j]; j++) {
                char *e = expand_variables_aux(fd->data[i].dependencies[j], *fd);
                if (e) {
                    free(fd->data[i].dependencies[j]);
                    fd->data[i].dependencies[j] = e;
                }
            }

            size_t orig_count = 0;
            while (fd->data[i].dependencies[orig_count]) orig_count++;

            char **new_deps = calloc(1, sizeof(char *));
            size_t new_count = 0;
            for (size_t j = 0; j < orig_count; j++) {
                char *entry = fd->data[i].dependencies[j];
                if (strchr(entry, ' ')) {
                    char *copy = strdup(entry);
                    char *tok = strtok(copy, " \t");
                    while (tok) {
                        if (tok[0] != '\0') {
                            char **tmp = realloc(new_deps, (new_count + 2) * sizeof(char *));
                            if (tmp) {
                                new_deps = tmp;
                                new_deps[new_count++] = strdup(tok);
                                new_deps[new_count] = NULL;
                            }
                        }
                        tok = strtok(NULL, " \t");
                    }
                    free(copy);
                    free(entry);
                } else if (entry[0] != '\0') {
                    char **tmp = realloc(new_deps, (new_count + 2) * sizeof(char *));
                    if (tmp) {
                        new_deps = tmp;
                        new_deps[new_count++] = entry;
                        new_deps[new_count] = NULL;
                    }
                } else {
                    free(entry);
                }
            }
            free(fd->data[i].dependencies);
            if (new_count == 0) {
                free(new_deps);
                fd->data[i].dependencies = NULL;
            } else {
                fd->data[i].dependencies = new_deps;
            }
        }
        if (fd->data[i].rule && strchr(fd->data[i].rule, ' ')) {
            char *rule_str = fd->data[i].rule;
            char *copy = strdup(rule_str);
            int ntok = 0;
            char *tmp2 = strdup(copy);
            char *tok = strtok(tmp2, " \t");
            while (tok) { ntok++; tok = strtok(NULL, " \t"); }
            free(tmp2);

            if (ntok > 1) {
                while (fd->size + ntok - 1 >= fd->cap) {
                    size_t nc = fd->cap * 2;
                    struct parse_data *t = realloc(fd->data, nc * sizeof(struct parse_data));
                    if (!t) { free(copy); goto skip_split; }
                    fd->data = t; fd->cap = nc;
                }
                tok = strtok(copy, " \t");
                free(fd->data[i].rule);
                fd->data[i].rule = strdup(tok);
                tok = strtok(NULL, " \t");
                while (tok) {
                    struct parse_data clone;
                    memset(&clone, 0, sizeof(clone));
                    clone.rule             = strdup(tok);
                    clone.variable         = strdup(fd->data[i].variable ? fd->data[i].variable : "");
                    clone.variables_value  = strdup(fd->data[i].variables_value ? fd->data[i].variables_value : "");
                    clone.comment          = strdup(fd->data[i].comment ? fd->data[i].comment : "");
                    clone.line_pos         = fd->data[i].line_pos;

                    if (fd->data[i].dependencies) {
                        size_t dc = 0;
                        while (fd->data[i].dependencies[dc]) dc++;
                        clone.dependencies = calloc(dc + 1, sizeof(char *));
                        for (size_t d = 0; d < dc; d++)
                            clone.dependencies[d] = strdup(fd->data[i].dependencies[d]);
                        clone.dependencies[dc] = NULL;
                    }

                    if (fd->data[i].commands) {
                        size_t cc = 0;
                        while (fd->data[i].commands[cc]) cc++;
                        clone.commands = calloc(cc + 1, sizeof(char *));
                        for (size_t c = 0; c < cc; c++)
                            clone.commands[c] = strdup(fd->data[i].commands[c]);
                        clone.commands[cc] = NULL;
                    }
                    fd->data[fd->size++] = clone;
                    tok = strtok(NULL, " \t");
                }
            }
            free(copy);
            skip_split:;
        }
    }
}

int main(int argc, char *argv[]) {
    int option;
    int err = 0;
    char *file = "Makefile";
    int p_option = 0;
    int h_option = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) { h_option = 1; break; }
    }

    if (h_option) {
        printf("Usage: minimake [OPTIONS] [TARGETS]\n"
               "\tOPTIONS can be:\n"
               "\t\t\"-h\"            to display the helper\n"
               "\t\t\"-p\"            to print the rules and variables\n"
               "\t\t\"-f\" <filename> to change the make config file, \"Makefile\" by default\n"
               "\tTARGETS are target name of makefile rule to execute\n");
        return 0;
    }

    while ((option = getopt(argc, argv, ":f:hp")) != -1) {
        switch (option) {
        case 'h': break;
        case 'f': {
            FILE *f = fopen(optarg, "r");
            if (!f) {
                printf("minimake: *** File does not exist. Stop.\n");
                return 3;
            }
            fclose(f);
            file = optarg;
            break;
        }
        case 'p':
            p_option = 1;
            break;
        case '?':
            printf("minimake: *** No rule to make target '%s'. Stop.\n", argv[optind - 1]);
            return 2;
        case ':':
            printf("minimake: *** No file provided. Stop.\n");
            return 2;
        }
    }

    {
        FILE *f = fopen(file, "r");
        if (!f) {
            printf("minimake: *** File does not exist. Stop.\n");
            return 3;
        }
        fclose(f);
    }

    char **lines = read_all_lines(file);
    clean_lines(lines);
    struct data fd = parsing(lines);

    expand_struct(&fd);

    if (p_option) {
        pretty_print(fd);
        for (size_t i = 0; i < fd.size; i++) free_parse_data(&fd.data[i]);
        free(fd.data);
        free_lines(lines);
        return 0;
    }

    char **targets = (optind < argc) ? &argv[optind] : NULL;
    err = execute_commands(fd, targets);

    for (size_t i = 0; i < fd.size; i++) free_parse_data(&fd.data[i]);
    free(fd.data);
    free_lines(lines);
    return err;
}

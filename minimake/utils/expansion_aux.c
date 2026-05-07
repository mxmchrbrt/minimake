#include "expansion.h"

#include <stdlib.h>
#include <string.h>

struct var *var_add(struct var *list, const char *key, const char *value) {
    struct var *node = malloc(sizeof(struct var));
    if (!node)
        return NULL;
    node->key = strdup(key);
    node->value = strdup(value);
    if (!node->key || !node->value) {
        free(node->key);
        free(node->value);
        free(node);
        return NULL;
    }
    node->next = list;
    return node;
}

const char *var_get(struct var *list, const char *key) {
    while (list)
    {
        if (strcmp(list->key, key) == 0)
            return list->value;
        list = list->next;
    }
    return NULL;
}

size_t var_size(struct var *list) {
    size_t n = 0;
    while (list)
    {
        n++;
        list = list->next;
    }
    return n;
}

int var_update(struct var *list, const char *key, const char *value) {
    while (list)
    {
        if (strcmp(list->key, key) == 0) {
            char *new_val = strdup(value);
            if (!new_val)
                return -1;
            free(list->value);
            list->value = new_val;
            return 0;
        }
        list = list->next;
    }
    return -1;
}

void var_free(struct var *list) {
    while (list) {
        struct var *next = list->next;
        free(list->key);
        free(list->value);
        free(list);
        list = next;
    }
}

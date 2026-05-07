#ifndef EXPANSION_H
#define EXPANSION_H

#include <stddef.h>

struct var {
    char *key;
    char *value;
    struct var *next;
};

struct var *var_add(struct var *list, const char *key, const char *value);
const char *var_get(struct var *list, const char *key);
size_t var_size(struct var *list);
int var_update(struct var *list, const char *key, const char *value);
void var_free(struct var *list);

#endif /* ! EXPANSION_H */

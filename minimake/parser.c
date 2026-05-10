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
    for (size_t i = 0; lines[i] != NULL; ++i) {
        free(lines[i]);
        lines[i] = NULL;
    }
    free(lines);
}

char **split(char *str) {
    if (!str) return NULL;

    size_t count = 1;
    for (size_t i = 0; str[i] != '\0'; i++)
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

struct data parsing(char **lines){
  struct data final_data;
  int found_rule = 0;
  final_data.size = 0;
  if (!lines) return final_data;

  struct parse_data *final_data_aux = calloc(1000,sizeof(struct parse_data)); 
  final_data.data = final_data_aux;
  final_data.size = 0;
  for (size_t i = 0; lines[i] != NULL; i++){
    struct parse_data data;
    strcpy(data.variable,"");
    strcpy(data.rule,"");
    strcpy(data.variables_value,"");
    strcpy(data.command,"");
    strcpy(data.comment,"");

    // comment
    char *p = strchr(lines[i], '#');
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

  //definition
    char *p3 = strchr(lines[i], '=');
    if (p3) {
    char *tmp = strdup(p3 + 2);
    if (tmp) {
      size_t len = strlen(tmp);
      if (len > 0 && tmp[len-1] == '\n') tmp[len-1] = '\0';
      strncpy(data.variables_value, tmp, sizeof(data.variables_value)-1);
      data.variables_value[sizeof(data.variables_value)-1] = '\0';
      free(tmp);
      }

    }  
  // dependencies 
    char *p4 = strchr(lines[i], ':');
    if (p4) {
      char *tmp = strdup(p4 + 1);  // +1 not +2, skip only the ':'
      if (tmp) {
          size_t len = strlen(tmp);
          if (len > 0 && tmp[len-1] == '\n') tmp[len-1] = '\0';
          char *deps_start = tmp;
          if (deps_start[0] == ' ') deps_start++;
          if (deps_start[0] != '\0') data.dependencies = split(deps_start);
          else data.dependencies = NULL;
        free(tmp);
    }
}

    for (size_t j = 0; lines[i][j] != '\0'; j++){
    //variable name
    if (strchr(lines[i],'=') != NULL){
      sscanf(lines[i],"%[a-zA-Z0-9]=",data.variable); 
    }


}    
    //rule
    char *p5 = strchr(lines[i], ':');
    if (p5) {
      found_rule = 1;
      size_t rule_len = p5 - lines[i];
      strncpy(data.rule, lines[i], rule_len);
      data.rule[rule_len] = '\0';
    }


   //command
    if (found_rule == 1) {
      char command_buf[1024] = "";
      size_t k = i + 1;
      int first = 1;
      while (lines[k] && lines[k][0] == '\t') {
        char *tmp = strdup(lines[k] + 1);
        if (tmp) {
            size_t len = strlen(tmp);
            if (len > 0 && tmp[len-1] == '\n') tmp[len-1] = '\0';
            if (!first) strncat(command_buf, " && ", sizeof(command_buf) - strlen(command_buf) - 1);
            strncat(command_buf, tmp, sizeof(command_buf) - strlen(command_buf) - 1);
            free(tmp);
            first = 0;
        }
        k++;
    }
      strncpy(data.command, command_buf, sizeof(data.command) - 1);
      data.command[sizeof(data.command) - 1] = '\0';
    } 
    found_rule = 0;
    final_data_aux[i] = data;
    final_data_aux[i].line_pos = i+1;
    data.dependencies = NULL;
    final_data.size++;

  }
    final_data.data = final_data_aux; 
    /*for (size_t i = 0; i < final_data.size; i++){
      printf("\nLines %d\n",final_data.data[i].line_pos);
      printf("variable = %s\nrule = %s\ndependencies = %s\nvariables_value = %s\ncommand = %s\ncomment = %s\n", final_data.data[i].variable,final_data.data[i].rule,final_data.data[i].dependencies,final_data.data[i].variables_value,final_data.data[i].command,final_data.data[i].comment);
    }*/
    return final_data;
}


char **clean_lines(char **lines){ //remove spaces except those after ':' or after a '\t' or after '=' 
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
      printf("(%s): ", final_data.data[i].rule);
      if (final_data.data[i].dependencies != NULL) {
          for (size_t j = 0; final_data.data[i].dependencies[j] != NULL; j++) {
              if (final_data.data[i].dependencies[j][0] != '\0')
                  printf("[%s] ", final_data.data[i].dependencies[j]);
          }
        }
      printf("\n");
      }
      if (final_data.data[i].command[0] != '\0') {
      char *cmd = strdup(final_data.data[i].command);
      char *token = cmd;
      char *next;
      while ((next = strstr(token, " && ")) != NULL) {
        *next = '\0';
        printf("\t'%s'\n", token);
        token = next + 4;
      }
      printf("\t'%s'\n", token);
      free(cmd);
      } 
    }
  }

char *expand_variables_aux(char *line, struct data final_data){
  if (!line) return NULL;
  size_t in_len = strlen(line);
  size_t cap = in_len + 1;
  char *out = calloc(cap,1);
  if (!out) return NULL;
  size_t pos = 0;

  for (size_t i = 0; line[i] != '\0'; ++i) {
    if (line[i] == '$' && line[i+1] == '{') {
      size_t j = i + 2;
      char name[256];
      size_t k = 0;
      while (line[j] != '\0' && line[j] != '}' && k + 1 < sizeof(name)) {
        name[k++] = line[j++];
      }
      name[k] = '\0';

      if (line[j] == '}' && k > 0) {
        const char *val = "";
        for (size_t z = 0; z < final_data.size; ++z) {
          if (strcmp(name, final_data.data[z].variable) == 0) {
            val = final_data.data[z].variables_value;
            break;
          }
        }
        size_t vlen = strlen(val);
        if (pos + vlen + 1 > cap) {
          size_t newcap = cap ? cap : 1;
          while (pos + vlen + 1 > newcap) newcap *= 2;
          char *tmp = realloc(out, newcap);
          if (!tmp) { free(out); return NULL; }
          out = tmp; cap = newcap;
        }
        memcpy(out + pos, val, vlen);
        pos += vlen;
        i = j;
        continue;
      }

    }

    if (pos + 2 > cap) {
      size_t newcap = cap ? cap * 2 : 64;
      char *tmp = realloc(out, newcap);
      if (!tmp){ free(out); return NULL; }
      out = tmp; cap = newcap;
    }
    out[pos++] = line[i];
  }
  out[pos] = '\0';
  return out;
}

char **expand_variables(char **lines, struct data final_data){
  if (!lines) return NULL;

  for (size_t i = 0; i < final_data.size; ++i){
    char *old = lines[i];
    char *new = expand_variables_aux(old, final_data);
    if (new) {
      free(old);
      lines[i] = new;
    }
  }

  return lines;
}       

size_t number_of_rules(struct data final_data) {
    size_t count = 0;
    for (size_t i = 0; i < final_data.size; i++) {
        if (final_data.data[i].rule[0] != '\0')
            count++;
    }
    return count;
}

void execute_commands(struct data final_data) {
    if (!final_data.data) return;

    size_t last_rule = number_of_rules(final_data);
    if (last_rule == 0) return;

    char *commands[100] = {NULL};

    size_t idx = 0;
    for (size_t i = 0; i < final_data.size && idx < 100; i++) {
        if (final_data.data[i].rule[0] != '\0' && final_data.data[i].command[0] != '\0') {
            commands[idx] = final_data.data[i].command;
            idx++;
        }
    }

    for (size_t i = idx; i > 0; i--) {
        if (!commands[i-1]) continue;
        printf("%s\n", commands[i-1]);
        fflush(stdout);

        pid_t pid = fork();
        if (pid == 0) {
            system(commands[i-1]);
            exit(0);
        } else if (pid > 0) {
            int status;
            if (waitpid(pid, &status, 0) == -1) {
                perror("waitpid");
            } else {
                if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                    fprintf(stderr, "minimake: command failed (exit %d): %s\n",
                            WEXITSTATUS(status), commands[i-1]);
                    return;
                }
            }
        } else {
            perror("fork");
            return;
        }
    }
}

#include "basics.h"

int main(int argc, char *argv[]){
  int option;

  int err = 0;
  char *file = "Makefile";
  int p_option = 0;
  int h_option = 0;
  struct data final_data;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-h") == 0) {
      h_option = 1;
      break;
    }
  }

  while ((option = getopt(argc,argv, ":f:hp")) != -1){
      switch (option){
        case 'h':
          printf("Usage: minimake [OPTIONS] [TARGETS]\n"
       "\tOPTIONS can be:\n"
       "\t\t\"-h\"            to display the helper\n"
       "\t\t\"-p\"            to print the rules and variables\n"
       "\t\t\"-f\" <filename> to change the make config file, \"Makefile\" by default\n"
       "\tTARGETS are target name of makefile rule to execute\n");
          break;
        case 'f':
          if (strcmp(optarg,"-h") == 0){
            printf("Please TYPE \"./minimake -h\" only to show options\n");
            break;
          }
          FILE *f = fopen(optarg, "r");
          if (!f) { printf("minimake: *** File does not exist. Stop.\n"); return 3; }
          else{
            fclose(f);
            file = optarg;
          }
          break;

        case 'p':;
          p_option = 1;
          if (!h_option){
            char **lines = read_all_lines(file);
            clean_lines(lines);
            final_data = parsing(lines);
      
            lines = expand_variables(lines, final_data);

            for (size_t i = 0; i < final_data.size; i++)
              free_lines(final_data.data[i].dependencies);
            free(final_data.data);

            struct data final_data_new = parsing(lines);
            pretty_print(final_data_new);

            for (size_t i = 0; i < final_data_new.size; i++)
              free_lines(final_data_new.data[i].dependencies);
            free(final_data_new.data);
            free_lines(lines);
          }
          break;

        case '?':
          printf("minimake: *** No rule to make target '%s'. Stop.\n", argv[optind-1]);
          return 2;
        case ':':
          printf("minimake: *** No file provided. Stop.\n");
          return 2;
      }
  }
  if (p_option == 0 && !h_option) {
    char **lines = read_all_lines(file);
    clean_lines(lines);
    final_data = parsing(lines);

    lines = expand_variables(lines, final_data);

    for (size_t i = 0; i < final_data.size; i++)
        free_lines(final_data.data[i].dependencies);
    free(final_data.data);

    struct data final_data_new = parsing(lines);

    char **targets = (optind < argc) ? &argv[optind] : NULL;
    err = execute_commands(final_data_new, targets);
    for (size_t i = 0; i < final_data_new.size; i++)
        free_lines(final_data_new.data[i].dependencies);
    free(final_data_new.data);
    free_lines(lines);
  } 
  return err;
}

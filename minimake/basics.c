#include "basics.h"

int main(int argc, char *argv[]){
  int option;
  char *file = "Makefile";
  int p_option = 0;
  struct data final_data;
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
            // use optarg
            fclose(f);
            file = optarg;
          } 
          break;

        case 'p':
          char **lines = read_all_lines(file);
          p_option = 1;
          clean_lines(lines);
          final_data = parsing(lines);
          pretty_print(final_data);
          free(final_data.data);
          free_lines(lines);
          break;

        case '?':
          printf("minimake: *** No rule to make target '%s'. Stop.\n", argv[optind-1]);
          return 2; 
        case ':':
          printf("minimake: *** No file provided. Stop.\n");
          return 2;  
          break;
      }    
  }
  if (p_option == 0){
    char **lines = read_all_lines(file);
    clean_lines(lines);
    final_data = parsing(lines);
    execute_commands(final_data);
    free(final_data.data);
    free_lines(lines);
  }
  return 0; 
}

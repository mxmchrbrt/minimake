#include "basics.h"
#include "parser.h"

int main(int argc, char *argv[]){
  int option;
  char *file = "Makefile";
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
          clean_lines(lines);
          parsing(lines);
          free_lines(lines);
          break;

        case '?':
          printf("minimake: *** No rule to make target '%s'. Stop.\n", argv[optind-1]);
          return 2; 
        case ':':
          printf("minimake: *** No file provided. Stop.\n");
          return 2;
      }
  }
  return 0; 
}

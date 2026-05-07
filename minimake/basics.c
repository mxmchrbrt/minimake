#include "basics.h"

void pretty_printf();
int parse_options(int argc, char *argv[]){
  int option;
   while ((option = getopt(argc,argv, ":f:hp")) != -1){
      switch (option){
        case 'f':
          if (access(optarg, F_OK) == -1){
            printf("File does not exist.\n");
            return 3;
          }
          else{
            // use optarg
            char* file = optarg;
          } 
          break;

        case 'h':
          printf("Usage: minimake [OPTIONS] [TARGETS]\n"
       "\tOPTIONS can be:\n"
       "\t\t\"-h\"            to display the helper\n"
       "\t\t\"-p\"            to print the rules and variables\n"
       "\t\t\"-f\" <filename> to change the make config file, \"Makefile\" by default\n"
       "\tTARGETS are target name of makefile rule to execute\n");
          break;

        case 'p':
          printf("Print\n");
          break;

        case '?':
          printf("minimake: *** No rule to make target '%s'. Stop.\n", argv[optind-1]);
          return 2; 
        case ':':
          printf("minimake: *** No file provided. Stop.\n");
          return 2;
      }
  }
  char *targets[] = calloc(optind-i,sizeof(char *));
  int j = 0;
  for (int i = optind; i < argc; i++){
    targets[j] = argv[i];
    j++;
  }
  return 0; 
}

int main(int argc, char *argv[]){
  parse_options(argc,argv);
  return 0;
}

Fix outline:
- Implement clean_lines to trim, ignore blanks/comments, and parse variable vs rule blocks.
- Implement pretty_print to output the exact expected format (quotes, section headers, parenthesized target name, tab + quoted command).
Add rule parsing + execution (default target "all" or specified targets) and run commands (fork/exec or system()) so "echo Toto" actually runs and prints "Toto".
Start by editing parser.c (pretty_print/clean_lines) and then add parsing/execution in basics.c after getopt.

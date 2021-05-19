/* System includes */
#include <stdio.h>
#include <unistd.h>
#include <string.h>

/* Parser header */
#include "grammar.tab.h"

/* Visitor declerations */
#include "Visitor_classes/Print_AST_visitor.h"
#include "Visitor_classes/Dec_before_use.h"

/* declared in either grammar.y or parser.l, needed by driver program */
extern FILE *yyin;
extern Stmt_dec *root;

/* 
    produces an object file for the specified file, has the following option flags

    -f <filename>:    required to run the program, produces object file for <filename>
    
    -d:               print the AST after parsing the input file
*/
int main(int argc, char **argv)
{

  int c;
  bool print_AST = false;
  bool file_specified = false;
  char *file_name;

  while ((c = getopt(argc, argv, "df:")) != -1)
  {
    switch (c)
    {
    case 'd':
      print_AST = true;
      break;
    case 'f':
      file_specified = true;
      file_name = strdup(optarg);
      break;
    case '?':
      printf("invalid command line option %c \n", c);
      break;
    }
  }

  /* check that the user's file is valid */
  yyin = fopen(file_name, "r");
  if (!yyin)
  {
    printf("error, invalid file name\n");
    return -1;
  }

  /* construct AST */
  yyparse();

  /* close file and free file name pointer after it's been parsed */
  free(file_name);
  fclose(yyin);

  /* declare our visitors */
  Print_AST_visitor print_visitor;
  Dec_before_use dec_visitor;

  /* print AST if -d flag passed to program */
  if (print_AST)
  {
    root->accept(print_visitor);
  }

  /* check variables are declared before they're used, and that no type mismatches exist  */
  root->accept(dec_visitor);

  /* Exit program with error if parse was uncessesful */
  if (!dec_visitor.parse_status())
  {
    std::cout << "Error while parsing, exiting" << std::endl;
    exit(1);
  }
}
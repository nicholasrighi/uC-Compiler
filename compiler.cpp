// System includes
#include <stdio.h>
#include <unistd.h>

// Parser header
#include "grammar.tab.h"

// Visitor declerations
#include "Visitor_classes/Print_AST_visitor.h"
#include "Visitor_classes/Dec_before_use.h"

/* declared in either grammar.y or parser.l, needed by driver program */
extern FILE *yyin;
extern Stmt_dec *root;

int main(int argc, char **argv)
{

  /* check that user has given us a file to parse */
  if (argc < 2)
  {
    printf("error, please supply a file to compile\n");
    return -1;
  }

  /* check that the user's file is valid */
  yyin = fopen(argv[1], "r");
  if (!yyin)
  {
    printf("error, invalid file name\n");
    return -1;
  }

  int c;
  bool print_AST = false;

  /* set command line flags */
  while ((c = getopt(argc, argv, "d")) != -1)
  {
    switch (c)
    {
    case 'd':
      print_AST = true;
      break;
    case '?':
      printf("invalid command line option %c \n", c);
      break;
    }
  }

  /* construct AST */
  yyparse();

  /* declare our visitors */
  Print_AST_visitor print_visitor;
  Dec_before_use dec_visitor;

  /* print AST if -d flag passed to program */
  if (print_AST)
  {
    root->accept(print_visitor);
  }

  /* check variables are declared before they're used */
  root->accept(dec_visitor);

  /* Exit program with error if parse was uncessesful */
  if (!dec_visitor.parse_status()) {
    exit(1);
  }
}
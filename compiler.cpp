/* Unix header */
#include <unistd.h>

/* System includes */
#include <string>

/* Parser header */
#include "grammar.tab.h"

/* Visitor declerations */
#include "Visitor_classes/Print_AST_visitor.h"
#include "Visitor_classes/Dec_before_use.h"

/* declared in either grammar.y or parser.l, needed by this program */
extern FILE *yyin;
extern Stmt_dec *root;

/* 
    produces an object file for the specified file, has the following option flags

    -f <filename>:    required to run the program, produces object file for <filename>
    
    -d:               print the AST after parsing the input file
*/
int main(int argc, char **argv)
{

  int option_flag;
  bool print_AST = false;
  bool file_specified = false;
  std::string file_name;

  /* parse input flags */
  while ((option_flag = getopt(argc, argv, "df:")) != -1)
  {
    switch (option_flag)
    {
    case 'd':
      print_AST = true;
      break;
    case 'f':
      file_specified = true;
      file_name = optarg;
      break;
    case '?':
      std::cout << "invalid command line option " << option_flag << std::endl;
      break;
    }
  }

  /* check that use gave us a file */
  if (!file_specified)
  {
    std::cout << "Must specify file with -f <filename> " << std::endl;
    return -1;
  }

  /* check that the user's file is valid */
  yyin = fopen(file_name.c_str(), "r");
  if (!yyin)
  {
    std::cout << "Error, invalid file name" << std::endl;
    return -1;
  }

  /* construct AST */
  yyparse();

  /* close file since we no longer need it after AST has been constructed */
  fclose(yyin);

  /* declare our visitors */
  Print_AST_visitor print_visitor;
  Dec_before_use dec_visitor;

  /* print AST if -d flag passed to program */
  if (print_AST)
  {
    root->accept(print_visitor);
  }

  /* check variables are declared before they're used, and that no type mismatches exist */
  root->accept(dec_visitor);

  /* Exit program with error if parse was uncessesful */
  if (!dec_visitor.parse_status())
  {
    std::cout << "Error while parsing, exiting" << std::endl;
    return -1;
  }
}
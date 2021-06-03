/* Unix header */
#include <unistd.h>

/* System includes */
#include <string>

/* Parser header */
#include "grammar.tab.h"

/* Visitor declerations */
#include "Visitor_classes/Print_AST_visitor.h"
#include "Visitor_classes/Dec_before_use.h"

#include "Supporting_classes/Symbol_table.h"

/* declared in either grammar.y or parser.l, needed by this program */
extern FILE *yyin;
extern Stmt_dec *root;
extern int yydebug;

/* 
    produces an object file for the specified file, has the following option flags

    -f <filename>:    required to run the program, produces object file for <filename>
    
    -d:               print the AST after parsing the input file

    -t:               enable bison output tracing
*/
int main(int argc, char **argv)
{

  int option_flag;
  bool print_AST = false;
  bool file_specified = false;
  std::string file_name;

  /* parse input flags */
  while ((option_flag = getopt(argc, argv, "tdf:")) != -1)
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
    case 't':
#ifdef YYDEBUG
      yydebug = 1;
#endif
      break;
    case '?':
      std::cout << "invalid command line option " << option_flag << std::endl;
      break;
    }
  }

  if (!file_specified)
  {
    std::cout << "Must specify file with -f <filename> " << std::endl;
    return -1;
  }

  yyin = fopen(file_name.c_str(), "r");
  if (!yyin)
  {
    std::cout << "Error, invalid file name" << std::endl;
    return -1;
  }

  /* construct AST */
  yyparse();

  fclose(yyin);

  std::list<Symbol_table> sym_table_list;

  Print_AST_visitor print_visitor;
  Dec_before_use dec_visitor(sym_table_list);

  if (print_AST)
  {
    root->accept(print_visitor);
  }

  root->accept(dec_visitor);

  if (!dec_visitor.global_parse_status())
  {
    std::cout << "Error while parsing, exiting" << std::endl;
    return -1;
  } 

  //dec_visitor.gen_3_code(root);
  //dec_visitor.print();
}
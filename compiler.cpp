/* Unix header */
#include <unistd.h>

/* System includes */
#include <string>

/* Parser header */
#include "grammar.tab.h"

/* Visitor declerations */
#include "Visitor_classes/Print_AST_visitor.h"
#include "Visitor_classes/Dec_before_use.h"

#include "Supporting_classes/Program_symbol_table.h"

/* declared in either grammar.y or parser.l, needed by this program */
extern FILE *yyin;
extern Stmt_dec *root;
extern int yydebug;


/* 
    produces an object file for the specified input file, has the following option flags

    -f <filename>:    required to run the program, produces object file for <filename>
    
    -d:               print the AST after parsing the input file

    -t:               enable bison output tracing

    -s:               print symbol table
*/
int main(int argc, char **argv)
{

  int option_flag;
  bool print_AST = false;
  bool file_specified = false;
  bool print_sym_table = false;
  std::string file_name;

  /* parse input flags */
  while ((option_flag = getopt(argc, argv, "stdf:")) != -1)
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
    case 's':
      print_sym_table = true;
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

  /*  Create symbol table list and allocate table for global variables */
  Program_symbol_table prog_sym_table;

  Print_AST_visitor print_visitor;
  Dec_before_use dec_visitor(prog_sym_table);
  Type_checker type_visitor(prog_sym_table);
  Return_checker return_visitor;

  if (print_AST)
  {
    root->accept(print_visitor);
  }

  root->accept(dec_visitor);

  if (print_sym_table) {
    prog_sym_table.print_prog_sym_table();
  }

  prog_sym_table.reset_sym_table_scopes();
  root->accept(type_visitor);
  root->accept(return_visitor);

  if (!dec_visitor.parse_status())
  {
    std::cout << "Error, var used before it was declared, exiting" << std::endl;
    return -1;
  } 

  if (!return_visitor.parse_status()) {
    std::cout << "Error, function has issues with return statements, exiting" << std::endl;
    return -1;
  }

  if (!type_visitor.parse_status()) {
    std::cout << "Error with type checker, exiting " << std::endl;
    return -1;
  }

  //dec_visitor.gen_3_code(root);
  //dec_visitor.print();
}
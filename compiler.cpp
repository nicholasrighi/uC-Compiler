/* Unix header */
#include <unistd.h>

/* System includes */
#include <string>
#include <fstream>

/* Parser header */
#include "grammar.tab.h"

/* Visitor declerations */
#include "Visitor_classes/Print_AST_visitor.h"
#include "Visitor_classes/Dec_before_use.h"
#include "Visitor_classes/Type_checker.h"
#include "Visitor_classes/Return_checker.h"
#include "Visitor_classes/Three_addr_gen.h"
#include "Visitor_classes/Three_addr_var.h"

/*  Symbol table definition */
#include "Supporting_classes/Program_symbol_table.h"
#include "Supporting_classes/Reg_allocator.h"

/* declared in either grammar.y or parser.l, needed by compiler.cpp program */
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
  bool input_file_specified = false;
  bool output_file_specified = false;
  bool print_sym_table = false;
  std::string input_file_name;
  std::string asm_file_name;

  /* parse input flags */
  while ((option_flag = getopt(argc, argv, "stdf:o:")) != -1)
  {
    switch (option_flag)
    {
    case 'd':
      print_AST = true;
      break;
    case 'f':
      input_file_specified = true;
      input_file_name = optarg;
      break;
    case 't':
#ifdef YYDEBUG
      yydebug = 1;
#endif
      break;
    case 's':
      print_sym_table = true;
      break;
    case 'o':
      asm_file_name = optarg;
      output_file_specified = true;
      break;
    case '?':
      std::cout << "invalid command line option " << option_flag << std::endl;
      break;
    }
  }

  if (!input_file_specified)
  {
    std::cout << "Must specify file with -f <filename>" << std::endl;
    return -1;
  }

  if (!output_file_specified) {
    std::cout << "Must specify output filename with -o <filename> " << std::endl;
    return -1;
  }

  yyin = fopen(input_file_name.c_str(), "r");
  if (!yyin)
  {
    std::cout << "Error, invalid file name" << std::endl;
    return -1;
  }

  /* construct AST */
  yyparse();

  fclose(yyin);

  Program_symbol_table prog_sym_table;
  std::vector<std::vector<three_addr_code_entry>> intermediate_code;
  std::ofstream debug_log(input_file_name + "_debug_log", std::ofstream::trunc);

  /*  Visitors for generating Three_addr_code and type checking input */
  Print_AST_visitor print_visitor;
  Dec_before_use dec_visitor(prog_sym_table);
  Type_checker type_visitor(prog_sym_table);
  Return_checker return_visitor;
  Three_addr_gen IR_generator(debug_log, prog_sym_table, intermediate_code);

  /*  Generates assembly file */
  Reg_allocator reg_allocator(asm_file_name, debug_log, prog_sym_table, intermediate_code);

  if (print_AST)
  {
    root->accept(print_visitor);
  }

  /*  Need to cll dec_visitor before printing symbol table to ensure symbol table is populated */
  root->accept(dec_visitor);

  if (print_sym_table)
  {
    prog_sym_table.print_prog_sym_table();
  }

  /*  Need to reset symbol table to ensure that future visitors start at the least nested scope */
  prog_sym_table.reset_sym_table_scopes();

  root->accept(type_visitor);
  root->accept(return_visitor);
  root->accept(IR_generator);

  if (!dec_visitor.parse_status())
  {
    std::cout << "Error, var used before it was declared, exiting" << std::endl;
    return -1;
  }

  if (!return_visitor.parse_status())
  {
    std::cout << "Error, function has issues with return statements, exiting" << std::endl;
    return -1;
  }

  if (!type_visitor.parse_status())
  {
    std::cout << "Error with type checker, exiting " << std::endl;
    return -1;
  }

  IR_generator.print_IR_code();
  reg_allocator.generate_asm_file();
}
#pragma once

// System includes
#include <list>

// Inherits from
#include "Abstract_visitor.h"

// local files
#include "../Supporting_classes/Program_symbol_table.h"

class Dec_before_use : public Abstract_visitor
{
public:
  /* 
     m_parse_flag is init to true since any dispatch method can set m_parse_flag to false
     during AST traversal. This will then result in parse_status() returning false, allowing
     the caller to determine the input file is invalid
  */
  Dec_before_use(Program_symbol_table& program_sym_table);

  void dispatch(Array_access &node) override;
  void dispatch(Array_dec &node) override;
  void dispatch(Binop_dec &node) override;
  void dispatch(Func_dec &node) override;
  void dispatch(Func_ref &node) override;
  void dispatch(If_dec &node) override;
  void dispatch(Number &node) override;
  void dispatch(Return_dec &node) override;
  void dispatch(Stmt_dec &node) override;
  void dispatch(Unop_dec &node) override;
  void dispatch(Var_dec &node) override;
  void dispatch(Var_ref &node) override;
  void dispatch(While_dec &node) override;

  /*  Returns true if the parser has parsed a file with no errors, otherwise false */
  bool parse_status();

private:

  /* 
      holds the status of the parser (true if parser sucess or if no parse has been run, 
      false if failure)
  */
  bool m_parse_flag = true;

  /*  indicates if variables should be defined as global */
  bool m_global_var_flag = true;

  /*  
      each entry in the list is the symbol table for a seperate function. The first entry in the list is the 
      symbol table for the global variables
  */
  Program_symbol_table& m_prog_sym_table;
};
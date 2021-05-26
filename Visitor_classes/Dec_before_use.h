#pragma once

// Inherits from
#include "Abstract_visitor.h"

// local files
#include "../Supporting_classes/Symbol_table.h"
#include "Type_checker.h"
#include "Return_checker.h"
#include "Three_addr_gen.h"

class Dec_before_use : public Abstract_visitor
{
public:
  /* 
     m_parse_flag is init to true since any dispatch method can set m_parse_flag to false
     during AST traversal. This will then result in parse_status() returning false, allowing
     the caller to determine the input file is invalid
  */
  Dec_before_use() : m_parse_flag(true), m_global_var_flag(true), m_bsp_offset(0), m_three_code_gen(&sym_table)
  {
    /* give the type checker access to the symbol table that the dec before use generates*/
    m_type_check_visitor.assign_sym_table(&sym_table);
    m_global_check_flag = true;
  }

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

  bool global_parse_status() {
    return m_global_check_flag;
  }

  void gen_3_code(Base_node* root) {
    root->accept(m_three_code_gen);
  }

  void print() {
    m_three_code_gen.print_IR_code();
  }

private:
  /* 
    returns true if no errors were detected when parsing, false otherwise
  */
  bool parse_status()
  {
    return m_parse_flag && m_type_check_visitor.parse_status() && m_return_checker.parse_status();
  }

  /* holds the status of the parser (true if parser sucess or if no parse has been run, false if failure) */
  bool m_parse_flag;

  /* holds the status if the parser ever failed */
  bool m_global_check_flag;

  /* holds variable declerations */
  Symbol_table sym_table;

  /*  type checker to verify that types are used correctly */
  Type_checker m_type_check_visitor;

  /*  ensures that all non void functions return a value through all paths of control */
  Return_checker m_return_checker;

  /*  generates an intermediate 3 address code from a valid AST */
  Three_addr_gen m_three_code_gen;

  /*  flag that determines if variables are defined as global or local */
  bool m_global_var_flag;

  /*  the offset the next variable is from the base pointer */
  int m_bsp_offset;
};
#pragma once

// Inherits from
#include "Abstract_visitor.h"

// local files
#include "../Supporting_classes/Symbol_table.h"
#include "Type_checker.h"
#include "Return_checker.h"

class Dec_before_use : public Abstract_visitor
{
public:
  /* 
     m_parse_flag is init to true since any dispatch method can set m_parse_flag to false
     during AST traversal. This will then result in parse_status() returning false, allowing
     the caller to determine the input file is invalid
  */
  Dec_before_use() : m_parse_flag(true)
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
};
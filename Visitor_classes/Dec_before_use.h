#pragma once

// Inherits from
#include "Abstract_visitor.h"

// local files
#include "../Supporting_classes/Symbol_table.h"

class Dec_before_use : public Abstract_visitor
{
public:
  /* 
     m_parse_flag is init to true since any dispatch method can set m_parse_flag to false
     during AST traversal. This will then result in parse_status() returning false, allowing
     the caller to determine the input file is invalid
  */
  Dec_before_use() : m_parse_flag(true) {}

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

  /* 
    returns true if no errors were detected when parsing, false otherwise
  */
  bool parse_status()
  {
    return m_parse_flag;
  }

private:
  Symbol_table sym_table;
  bool m_parse_flag;
};
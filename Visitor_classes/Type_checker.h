#pragma once

// inherits from
#include "Abstract_visitor.h"

// system includes
#include <iostream>

// local includes
#include "../AST_classes/Var_type_decs.h"
#include "../Supporting_classes/Symbol_table.h"

class Type_checker : public Abstract_visitor
{
public:
  Type_checker() : m_parse_flag(true) {}

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

  void assign_sym_table(Symbol_table* sym_table) {m_sym_table = sym_table;}

private:
  /* holds the return type of the node currently being examined */
  Ret_type m_ret_type;

  /* holds the status of the parser (true if parser sucess or if no parse has been run, false if failure) */
  bool m_parse_flag;

  /* holds variable declerations */
  Symbol_table* m_sym_table;

  /* holds the return type of the function currently being examined */
  Ret_type m_cur_func_ret_type;
};

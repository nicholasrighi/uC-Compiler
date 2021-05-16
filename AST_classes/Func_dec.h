#pragma once

#include "Var_dec.h"
#include "Stmt_dec.h"

class Func_dec : public Var_dec
{
public:
  Func_dec(Var_ref *name, Ret_type return_type, Stmt_dec *args, Stmt_dec *func_body)
      : Var_dec(name, return_type, Object_type::FUNC), m_args(args), m_func_body(func_body) {}

  void accept(Abstract_visitor &visitor) override
  {
    visitor.dispatch(*this);
  }

  /* the Stmt_dec contains a list of var_decs and array_decs */
  Stmt_dec *m_args;

  /* pointer to inside of function body */
  Stmt_dec *m_func_body;
};
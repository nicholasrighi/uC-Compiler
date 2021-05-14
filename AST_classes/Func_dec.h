#pragma once

#include "Var_dec.h"
#include "Stmt_dec.h"

// since a function needs a name and a type (return type), having the function inherit from
// Var_dec means that the name and type are already taken care of by that class
class Func_dec : public Var_dec
{
public:
  Func_dec(Var_ref *name, Var_type return_type, Stmt_dec *args, Stmt_dec *func_body)
      : Var_dec(name, return_type), m_args(args), m_func_body(func_body) {}

  void accept(Abstract_visitor &visitor) override
  {
    visitor.dispatch(*this);
  }

  Stmt_dec *m_args;
  Stmt_dec *m_func_body;
};
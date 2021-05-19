#pragma once

#include "Stmt_dec.h"
#include "Var_ref.h"

class Func_ref : public Base_node
{
public:
  Func_ref(Var_ref *name, Stmt_dec *args) : m_var(name), m_args(args) {}

  void accept(Abstract_visitor &visitor) override
  {
    visitor.dispatch(*this);
  }

  Var_ref *m_var;
  Stmt_dec *m_args;
};
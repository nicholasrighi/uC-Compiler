#pragma once

#include "Stmt_dec.h"
#include "Var_ref.h"

class Func_ref : public Base_node
{
public:
  Func_ref(Var_ref *name, Base_node *args) : m_var(name), m_args(args) {}

  void accept(Abstract_visitor &visitor) override
  {
    visitor.dispatch(*this);
  }

  Var_ref *m_var;
  Base_node *m_args;
};
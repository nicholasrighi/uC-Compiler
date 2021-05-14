#pragma once

#include "Base_node.h"
#include "Var_ref.h"
#include "Stmt_dec.h"

class Array_ref : public Base_node
{
public:
  Array_ref(Var_ref *name, Stmt_dec *access_index) : m_var(name), m_access_index(access_index) {}

  void accept(Abstract_visitor &visitor) override
  {
    visitor.dispatch(*this);
  }
  Var_ref *m_var;
  Stmt_dec *m_access_index;
};
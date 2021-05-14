#pragma once

#include "Base_node.h"
#include "Var_ref.h"

class Array_access : public Base_node
{
public:
  Array_access(Var_ref *name, Base_node *access_index) : m_var(name), m_access_index(access_index) {}

  void accept(Abstract_visitor &visitor)
  {
    visitor.dispatch(*this);
  }

  Var_ref *m_var;
  Base_node *m_access_index;
};
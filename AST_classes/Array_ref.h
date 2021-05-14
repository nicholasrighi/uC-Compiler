#pragma once

#include "Base_node.h"
#include "Var_ref.h"

class Array_ref : public Var_dec
{
public:
  Array_ref(Var_ref *name, Var_type type) : Var_dec(name, type) {} 

  void accept(Abstract_visitor &visitor) override
  {
    visitor.dispatch(*this);
  }
};
#pragma once

// Inherits from
#include "Var_dec.h"

class Array_dec : public Var_dec
{
public:
  Array_dec(Var_ref *name, Ret_type type, int size) : Var_dec(name, type, Object_type::ARRAY), m_array_size(size) {}

  void accept(Abstract_visitor &visitor) override
  {
    visitor.dispatch(*this);
  }

  /* needed to determine how much space in memory to reserve */
  int m_array_size;
};
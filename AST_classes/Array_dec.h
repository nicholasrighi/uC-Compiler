#pragma once

#include "Var_dec.h"

// since an array needs to store the name, type, and size information, it makes more sense to
// have it inherit from Var_dec as opposed to Var_ref, since Var_dec already specifies name
// and type, meaning we only need to add one additional field
class Array_dec : public Var_dec
{
public:
  Array_dec(Var_ref *name, Var_type type, int size) : Var_dec(name, type), m_array_size(size) {}

  void accept(Abstract_visitor &visitor) override
  {
    visitor.dispatch(*this);
  }
  int m_array_size;
};
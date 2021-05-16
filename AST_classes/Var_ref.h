#pragma once

#include <string>
#include "Base_node.h"

class Var_ref : public Base_node
{
public:
  // Construct variable reference for a non array variable. m_ref_type can't be known when a ref_type object
  // is returned by the lexer, so m_ref_type is set to ERROR
  Var_ref(char *name) : m_name(name) {}

  void accept(Abstract_visitor& visitor) override {
    visitor.dispatch(*this);
  }
  std::string m_name;
};
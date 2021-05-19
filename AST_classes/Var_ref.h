#pragma once

#include <string>
#include "Base_node.h"

class Var_ref : public Base_node
{
public:
  Var_ref(char *name) : m_name(name) {}

  void accept(Abstract_visitor& visitor) override {
    visitor.dispatch(*this);
  }
  std::string m_name;
};
#pragma once

#include <string>

#include "Base_node.h"

class Unop_dec : public Base_node
{
public:
  Unop_dec(char *op) : m_op(op) {}

  void add_expression(Base_node* exp) {m_exp = exp;}

  void accept(Abstract_visitor& visitor) override {
    visitor.dispatch(*this);
  }
  std::string m_op;
  Base_node *m_exp;
};
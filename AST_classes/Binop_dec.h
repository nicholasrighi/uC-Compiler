#pragma once

#include <string>
#include "Base_node.h"

class Binop_dec : public Base_node
{
public:
  Binop_dec(char *op) : m_op(op) {}
  void add_left_exp(Base_node* left) {m_left = left;}
  void add_right_exp(Base_node* right) {m_right = right;}

  void accept(Abstract_visitor& visitor) {
    visitor.dispatch(*this);
  }
  std::string m_op;
  Base_node* m_left;
  Base_node* m_right;
};
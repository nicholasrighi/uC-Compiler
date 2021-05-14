#pragma once

#include <list>
#include "Base_node.h"
#include "../Visitor_classes/Abstract_visitor.h"

class Stmt_dec : public Base_node
{
public:
  void add_expression_back(Base_node* exp) { m_sub_expressions.push_back(exp); }
  void add_expression_front(Base_node* exp) { m_sub_expressions.push_front(exp); }

  void accept(Abstract_visitor& visitor) override {
    visitor.dispatch(*this);
  }

  std::list<Base_node*> m_sub_expressions;
};
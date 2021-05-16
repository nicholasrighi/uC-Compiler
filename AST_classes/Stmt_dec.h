#pragma once

// Inherits from
#include "Base_node.h"

// System includes
#include <list>

class Stmt_dec : public Base_node
{
public:
  Stmt_dec() : m_new_scope(false) {}

  void add_expression_back(Base_node *exp) { m_sub_expressions.push_back(exp); }
  void add_expression_front(Base_node *exp) { m_sub_expressions.push_front(exp); }

  void accept(Abstract_visitor &visitor) override
  {
    visitor.dispatch(*this);
  }

  std::list<Base_node *> m_sub_expressions;
  bool m_new_scope;
};
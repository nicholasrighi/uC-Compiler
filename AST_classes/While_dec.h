#pragma once

#include "Stmt_dec.h"
#include "Base_node.h"

class While_dec : public Base_node
{
public:
  While_dec(Base_node *cond, Stmt_dec *body) : m_cond(cond), m_body(body) {}
  void accept(Abstract_visitor &visitor) override
  {
    visitor.dispatch(*this);
  }

  Base_node *m_cond;
  Base_node *m_body;
};
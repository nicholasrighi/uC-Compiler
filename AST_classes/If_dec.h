#pragma once

// local references
#include "Stmt_dec.h"
#include "Base_node.h"

class If_dec : public Base_node
{
public:
  If_dec(Base_node *cond, Stmt_dec *stmt_if_true, Stmt_dec *stmt_if_false)
      : m_cond(cond), m_stmt_if_true(stmt_if_true), m_stmt_if_false(stmt_if_false) {}

  void accept(Abstract_visitor &visitor) override
  {
    visitor.dispatch(*this);
  }

  Base_node *m_cond;
  Stmt_dec *m_stmt_if_true;
  Stmt_dec *m_stmt_if_false;
};
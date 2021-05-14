#pragma once

#include "Base_node.h"

class Return_dec : public Base_node
{
public:
  Return_dec(Base_node *child) : m_return_value(child) {}
  void accept(Abstract_visitor &visitor) override
  {
    visitor.dispatch(*this);
  }

  Base_node *m_return_value;
};
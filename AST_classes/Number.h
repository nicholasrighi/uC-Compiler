#pragma once

#include "Base_node.h"

class Number : public Base_node
{
public:
  Number(int value) : m_value(value) {}

  void accept(Abstract_visitor& visitor) override {
    visitor.dispatch(*this);
  }
  int m_value;
};
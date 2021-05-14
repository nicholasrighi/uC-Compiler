#pragma once

#include "../Visitor_classes/Abstract_visitor.h"

#include "../Visitor_classes/Abstract_visitor.h"
class Base_node
{
public:
  virtual void accept(Abstract_visitor &visitor) = 0;
};
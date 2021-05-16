#pragma once

// Abstract visitor header
#include "../Visitor_classes/Abstract_visitor.h"

// Header for variable types
#include "Var_type_decs.h"

class Base_node
{
public:
  virtual void accept(Abstract_visitor &visitor) = 0;
};
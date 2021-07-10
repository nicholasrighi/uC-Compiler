#pragma once

// Inherits from
#include "Base_node.h"

// System includes
#include <vector>

class Stmt_dec : public Base_node
{
public:
  Stmt_dec();

  ~Stmt_dec();

  void add_expression_back(Base_node *exp);
  void add_expression_front(Base_node *exp);

  void accept(Abstract_visitor &visitor) override;

  /* copies all base_node pointers stored in one Stmt_dec class to this class */
  void append_stmt_list(Stmt_dec *other);

  /* 
    copies all base_node pointer stored in a seperate Stmt_dec class to the front
    of m_sub_expressions 
  */
  void prepend_stmt_list(Stmt_dec *other);

  /*  The subexpressions this stmt_node contains */
  std::vector<Base_node *> m_sub_expressions;

  /*  Indicates if the stmt_node is part of a new scope */
  bool m_new_scope;
};
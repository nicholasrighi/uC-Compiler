#include "Stmt_dec.h"

Stmt_dec::Stmt_dec() : m_new_scope(false) {}

Stmt_dec::~Stmt_dec()
{
  for (Base_node *cur_node_ptr : m_sub_expressions)
  {
    delete (cur_node_ptr);
  }
}

void Stmt_dec::add_expression_back(Base_node *exp)
{
  m_sub_expressions.push_back(exp);
}

void Stmt_dec::add_expression_front(Base_node *exp)
{
  m_sub_expressions.insert(m_sub_expressions.begin(), exp);
}

void Stmt_dec::accept(Abstract_visitor &visitor)
{
  visitor.dispatch(*this);
}

/* copies all base_node pointers stored in one Stmt_dec class to this class */
void Stmt_dec::append_stmt_list(Stmt_dec *other)
{
  for (Base_node *sub_stmt : other->m_sub_expressions)
  {
    m_sub_expressions.push_back(sub_stmt);
  }
  other->m_sub_expressions.clear();
}

/* 
    copies all base_node pointer stored in a seperate Stmt_dec class to the front
    of m_sub_expressions 
  */
void Stmt_dec::prepend_stmt_list(Stmt_dec *other)
{
  /* 
      swapping the vectors means that the base_nodes that were stored in other (ie. the ones we want at the 
      front of this.m_sub_expressions) are now at the front of m_sub_expressions. We can then call append_stmt_list
      to put the rest of the nodes at the back of m_sub_expression
    */
  std::swap(m_sub_expressions, other->m_sub_expressions);
  append_stmt_list(other);
}

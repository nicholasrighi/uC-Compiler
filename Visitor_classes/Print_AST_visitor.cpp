// implements
#include "Print_AST_visitor.h"

// system includes
#include <iostream>

// AST nodes
#include "../AST_classes/Array_access.h"
#include "../AST_classes/Array_dec.h"
#include "../AST_classes/Binop_dec.h"
#include "../AST_classes/Func_dec.h"
#include "../AST_classes/Func_ref.h"
#include "../AST_classes/If_dec.h"
#include "../AST_classes/Number.h"
#include "../AST_classes/Return_dec.h"
#include "../AST_classes/Stmt_dec.h"
#include "../AST_classes/Unop_dec.h"
#include "../AST_classes/Var_dec.h"
#include "../AST_classes/Var_ref.h"
#include "../AST_classes/While_dec.h"

/* determines the number of tabs that are printed before the AST node prints */
int indent_level = 0;

void print_tabs(int n)
{
  for (int i = 0; i < 2*n; i++)
  {
    std::cout << " ";
  }
}

void add_indent_level()
{
  indent_level++;
  print_tabs(indent_level);
}

void insert_indent() {
  print_tabs(indent_level);
}

void remove_indent_level()
{
  indent_level--;
}

void Print_AST_visitor::dispatch(Array_access &node)
{
  add_indent_level();

  std::cout << "Accessing array " << node.m_var->m_name << " at:" << std::endl;

  /* evaluate array access expression */
  node.m_access_index->accept(*this);

  remove_indent_level();
}

void Print_AST_visitor::dispatch(Array_dec &node)
{
  add_indent_level();

  std::cout << "Declared array " << node.get_name() << " of size " << node.m_array_size << std::endl;

  remove_indent_level();
}

void Print_AST_visitor::dispatch(Binop_dec &node)
{
  add_indent_level();

  std::cout << "Binop with operator " << node.m_op << std::endl;

  node.m_left->accept(*this);
  node.m_right->accept(*this);

  remove_indent_level();
}

void Print_AST_visitor::dispatch(Func_dec &node)
{
  /* print function name*/
  std::cout << "function " << node.get_name() << " defined" << std::endl;

  add_indent_level();

  std::cout << "args: " << std::endl;

  /* since it's possible to have "void" as an input, we need to check
   if the function has any arguments*/
  if (node.m_args == nullptr)
  {
    add_indent_level();
    std::cout << "void" << std::endl;
    remove_indent_level();
  }
  else
  {
    node.m_args->accept(*this);
  }

  insert_indent();

  std::cout << "func body:" << std::endl;

  /* print function body */
  node.m_func_body->accept(*this);

  remove_indent_level();

  std::cout << std::endl;
}

void Print_AST_visitor::dispatch(Func_ref &node)
{
  add_indent_level();

  std::cout << "calling function " << node.m_var->m_name << " with args:" << std::endl;

  if (node.m_args == nullptr) {
    add_indent_level();
    std::cout << "no args" << std::endl;
    remove_indent_level();
  } else {
    node.m_args->accept(*this);
  }

  remove_indent_level();
}

void Print_AST_visitor::dispatch(If_dec &node)
{
  add_indent_level();

  std::cout << "If statement with condition:" << std::endl;

  node.m_cond->accept(*this);

  insert_indent();

  std::cout << "If statement is true, executing:" << std::endl;

  node.m_stmt_if_true->accept(*this);

  insert_indent();

  std::cout << "If statement is false, executing:" << std::endl;

  node.m_stmt_if_false->accept(*this);

  remove_indent_level();
}

void Print_AST_visitor::dispatch(Number &node)
{
  add_indent_level();
  std::cout << "Constant with value " << node.m_value << std::endl;
  remove_indent_level();
}

void Print_AST_visitor::dispatch(Return_dec &node)
{
  add_indent_level();

  std::cout << "Returning:" << std::endl;

  node.m_return_value->accept(*this);

  remove_indent_level();
}

void Print_AST_visitor::dispatch(Stmt_dec &node)
{
  for (auto &child_node : node.m_sub_expressions)
  {
    child_node->accept(*this);
  }
}

void Print_AST_visitor::dispatch(Unop_dec &node)
{
  add_indent_level();

  std::cout << "Unop with operator " << node.m_op << std::endl;

  node.m_exp->accept(*this);

  remove_indent_level();
}

void Print_AST_visitor::dispatch(Var_dec &node)
{
  add_indent_level();

  std::cout << node.m_var->m_name << " declared with type " << node.get_type_string() << std::endl;

  remove_indent_level();
}

void Print_AST_visitor::dispatch(Var_ref &node)
{
  add_indent_level();

  std::cout << "Referencing variable " << node.m_name << std::endl;

  remove_indent_level();
}

void Print_AST_visitor::dispatch(While_dec &node)
{
  add_indent_level();

  std::cout << "Found while statmement with condition:" << std::endl;

  node.m_cond->accept(*this);

  insert_indent();

  std::cout << " and body " << std::endl;

  node.m_body->accept(*this);

  remove_indent_level();
}
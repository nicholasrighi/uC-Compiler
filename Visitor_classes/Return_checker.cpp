// Implements
#include "Return_checker.h"

// System includes
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

void Return_checker::dispatch(Array_access &node)
{
  /* do nothing */
}

void Return_checker::dispatch(Array_dec &node)
{
  /* do nothing */
}

void Return_checker::dispatch(Binop_dec &node)
{
  /* do nothing */
}

/*
  Checks all sub statements in a function's body for return statements
*/
void Return_checker::dispatch(Func_dec &node)
{
  /* set both bools to false to reset the visitor */
  m_found_return_for_all_paths = false;
  m_returns = false;

  node.m_func_body->accept(*this);

  if (!m_found_return_for_all_paths)
  {
    std::cout << "Error, function " << node.get_name() << " doesn't return through all control flow " << std::endl;
  }
}

void Return_checker::dispatch(Func_ref &node)
{
  /* do nothing */
}

/*
  Checks if all paths of control flow reach a return statement
*/
void Return_checker::dispatch(If_dec &node)
{
  /* determine if sub statements are guaranteed to have return statement through all control flow */
  node.m_stmt_if_true->accept(*this);
  bool if_true_has_return = m_returns;

  bool if_false_has_return;

  /* if there's no else part, then we don't have a guaranteed control flow (since the if statement might not be executed) */
  if (node.m_stmt_if_false == nullptr)
  {
    m_returns = false;
    return;
  }
  else
  {
    node.m_stmt_if_false->accept(*this);
    if_false_has_return = m_returns;
  }

  /* return if both statements have guaranteed return statement */
  m_returns = if_true_has_return && if_false_has_return;
}

void Return_checker::dispatch(Number &node)
{
  /* do nothing */
}

/*
  Sets m_returns = true, since a return statement has been found
*/
void Return_checker::dispatch(Return_dec &node)
{
  m_returns = true;
}

/*
  Recursively check that all sub statements return through all control flow paths
*/
void Return_checker::dispatch(Stmt_dec &node)
{
  /* iterating backwards through the list ensures that we find the return statements farthest down the input file */
  for (auto sub_stmt = node.m_sub_expressions.rbegin(); sub_stmt != node.m_sub_expressions.rend(); sub_stmt++)
  {
    (*sub_stmt)->accept(*this);

    /* if we've found a guarenteed return path then return that fact to that caller */
    if (m_returns)
    {
      m_found_return_for_all_paths = true;
      return;
    }
  }
  /* if we exit the loop then there are paths that return false, record that fact */
  m_found_return_for_all_paths = false;
  m_returns = false;
}

void Return_checker::dispatch(Unop_dec &node)
{
  /* do nothing */
}

void Return_checker::dispatch(Var_dec &node)
{
  /* do nothing */
}

void Return_checker::dispatch(Var_ref &node)
{
  /* do nothing */
}

/*
  Since we can't guarantee that a while loop will ever be entered, don't check it's body for 
  return statements, since if a while loop is the last part of a program the body could 
  never be executed
*/
void Return_checker::dispatch(While_dec &node)
{
  /* do nothing */
}

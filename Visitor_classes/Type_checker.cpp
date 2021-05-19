// Implements
#include "Type_checker.h"

// System includes
#include <optional>

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

/*
    Verifies that the expression being used to access the array is an integer.
*/
void Type_checker::dispatch(Array_access &node)
{
  node.m_access_index->accept(*this);

  if (m_ret_type != Ret_type::INT)
  {
    std::cout << "Error, expected an integer expression for indexing, found a " << type_to_string(m_ret_type) << " type instead" << std::endl;
    m_parse_flag = false;
  }
}

/*
  Verifies that the array isn't being declared with a type void
*/
void Type_checker::dispatch(Array_dec &node)
{
  if (node.m_var_type == Ret_type::VOID)
  {
    std::cout << "Error, array '" << node.m_var->m_name << "' declared as VOID" << std::endl;
    m_parse_flag = false;
  }
}

/*
    Verify that the left and right expressions of the binop are the same, and are compatible.
    In reality this just means that they're both ints
*/
void Type_checker::dispatch(Binop_dec &node)
{

  /* visit left node and save result */
  node.m_left->accept(*this);
  Ret_type left_type = m_ret_type;

  /* check that left type is INT */
  if (left_type != Ret_type::INT)
  {
    std::cout << "Error, left expression of operator '" << node.m_op << "' is " << type_to_string(left_type) << " instead of INT" << std::endl;
    m_parse_flag = false;
  }

  /* visit right node and save result */
  node.m_right->accept(*this);
  Ret_type right_type = m_ret_type;

  /* check that right type is INT */
  if (right_type != Ret_type::INT)
  {
    std::cout << "Error, right expression of operator '" << node.m_op << "' is " << type_to_string(right_type) << " instead of INT" << std::endl;
    m_parse_flag = false;
  }
}

/*
    Verify that the type of function argument is non void
*/
void Type_checker::dispatch(Func_dec &node)
{
  /* verify that none of the function arguments are declared with type void */
  if (node.m_args != nullptr)
  {
    for (auto &arg : node.m_args->m_sub_expressions)
    {
      arg->accept(*this);
      if (m_ret_type == Ret_type::VOID)
      {
        std::cout << "Error, function '" << node.m_var->m_name << "' has argument declared an argument of type VOID" << std::endl;
        m_parse_flag = false;
      }
    }
  }

  /* verify that the function body has valid typing */
  node.m_func_body->accept(*this);
}

/*
*/
void Type_checker::dispatch(Func_ref &node)
{
  /* 
     get var dec out of symbol table. Since this visitor runs after the dec_before_use visitor,
     we know that the variable exists 
  */
  std::optional<Var_dec *> func_dec = m_sym_table->get_var_dec(node.m_var->m_name);

  /* get arguments for function decleration and reference, need to compare them to determine if function is being called correctly */
  Stmt_dec *func_dec_args = reinterpret_cast<Func_dec *>(*func_dec)->m_args;
  Stmt_dec *func_ref_args = node.m_args;

  /* if both function arguments are nullptr, then the function is being called correctly */
  if (func_ref_args == nullptr && func_ref_args == nullptr)
  {
    return;
  }

  /* check if only one of the arguments is null; if only one is null, function is being called incorrectly error */
  if ((func_dec_args == nullptr && func_ref_args != nullptr) || (func_dec_args != nullptr && func_ref_args == nullptr))
  {
    std::cout << "Error, calling function " << node.m_var->m_name << " with invalid arguments" << std::endl;
    m_parse_flag = false;
    return;
  }

  /* check that the number of arugments provided is the same as the number of arguments declared */
  if (func_dec_args->m_sub_expressions.size() != func_ref_args->m_sub_expressions.size())
  {
    std::cout << "Error, function '" << node.m_var->m_name << "' called with " << func_ref_args->m_sub_expressions.size() << " arguments, " << func_dec_args->m_sub_expressions.size() << " arguments expected" << std::endl;
    m_parse_flag = false;
  }

  /* check that the provided arguments have the same type as the declared arguments */

  auto it1 = func_dec_args->m_sub_expressions.begin();
  /* if we get here we know that the number of decalred and provided function arguments are the same, only need one end iterator*/
  auto end1 = func_dec_args->m_sub_expressions.end();
  auto it2 = func_ref_args->m_sub_expressions.begin();

  for (; it1 != end1; it1++, it2++)
  {
    (*it1)->accept(*this);
    Ret_type declared_arg_type = m_ret_type;
    (*it2)->accept(*this);
    Ret_type provided_arg_type = m_ret_type;

    if (declared_arg_type != provided_arg_type)
    {
      std::cout << "Error calling function '" << node.m_var->m_name << "': Expected argument type " << type_to_string(declared_arg_type)
                << " but found argument type " << type_to_string(provided_arg_type) << std::endl;
      m_parse_flag = false;
    }
  }
}

/*
   Verifies that the condition type of the if statement evaluates to non void, then
   checks the body of the if statement to check that it has valid typing
*/
void Type_checker::dispatch(If_dec &node)
{
  /* check that condition doesn't evalate to VOID */
  node.m_cond->accept(*this);

  if (m_ret_type == Ret_type::VOID)
  {
    std::cout << "Error, condition of if statement evaluted to VOID" << std::endl;
    m_parse_flag = false;
  }

  /* check body of if statement */
  node.m_stmt_if_true->accept(*this);

  /* check body of else statement if the else statement exists */
  if (node.m_stmt_if_false != nullptr)
  {
    node.m_stmt_if_false->accept(*this);
  }
}

/*
  Sets m_ret_type to INT
*/
void Type_checker::dispatch(Number &node)
{
  m_ret_type = Ret_type::INT;
}

/* 
  A different visitor will ensure that the return type of the function matches the declared return type,
  so no checks needed for return node
*/
void Type_checker::dispatch(Return_dec &node)
{
  /* do nothing */
}

/*
  Verifies legal typing for all child nodes
*/
void Type_checker::dispatch(Stmt_dec &node)
{
  for (auto &child_node : node.m_sub_expressions)
  {
    child_node->accept(*this);
  }
}

/*
  Verify that the expression for the unop is an integer
*/
void Type_checker::dispatch(Unop_dec &node)
{

  node.m_exp->accept(*this);

  if (m_ret_type != Ret_type::INT)
  {
    std::cout << "Error, expectd an int, but instead found type " << type_to_string(m_ret_type) << std::endl;
    m_parse_flag = false;
  }
}

/*
  Verifies that the variable isn't being declared as void
*/
void Type_checker::dispatch(Var_dec &node)
{
  m_ret_type = node.m_var_type;
}

/*
  Sets m_ret_type to the type that the symbol table has for that variable
*/
void Type_checker::dispatch(Var_ref &node)
{
  std::optional<Var_dec *> ref = m_sym_table->get_var_dec(node.m_name);
  m_ret_type = (*ref)->m_var_type;
}

/*
  Verifies that the conditional part of the while loop is non void. Check that the 
  body of the loop has correct type matching
*/
void Type_checker::dispatch(While_dec &node)
{

  /* check type of conditonal statement */
  node.m_cond->accept(*this);

  if (m_ret_type == Ret_type::VOID)
  {
    std::cout << "Error, void type for while loop expression" << std::endl;
    m_parse_flag = false;
  }

  /* type check body of loop */
  if (node.m_body != nullptr)
  {
    node.m_body->accept(*this);
  }
}

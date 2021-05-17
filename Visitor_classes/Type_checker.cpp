// Implements
#include "Type_checker.h"

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
    Checks that the expression being used to access the array is an integer. If not report error
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

*/
void Type_checker::dispatch(Array_dec &node) {}

/*
    Checks that the left and right expressions of the binop are the same, and are compatible.
    In reality this just means that they're both ints
*/
void Type_checker::dispatch(Binop_dec &node)
{

  /* visit left node and save result */
  node.m_left->accept(*this);
  Ret_type left_type = m_ret_type;

  /* visit right node and save result */
  node.m_right->accept(*this);
  Ret_type right_type = m_ret_type;

  if (left_type != right_type)
  {
    m_parse_flag = false;
    std::cout << "Error, mismatch type with operator " << node.m_op << std::endl;
    std::cout << "Left side has type " << type_to_string(left_type) << std::endl;
    std::cout << "Right side has type " << type_to_string(right_type) << std::endl;
  }
}

/*

*/
void Type_checker::dispatch(Func_dec &node) {}

/*

*/
void Type_checker::dispatch(Func_ref &node) {}

/*

*/
void Type_checker::dispatch(If_dec &node) {}

/*
  Number means that we set m_ret_type to INT
*/
void Type_checker::dispatch(Number &node)
{
  m_ret_type = Ret_type::INT;
}

/*

*/
void Type_checker::dispatch(Return_dec &node) {}

/*
    Statements have no type, but we need to check the type of each child node
*/
void Type_checker::dispatch(Stmt_dec &node)
{
  for (auto &child_node : node.m_sub_expressions)
  {
    child_node->accept(*this);
  }
}

/*
    Unop only work on integers, so need to make sure that the expression has type integer
*/
void Type_checker::dispatch(Unop_dec &node)
{

  node.m_exp->accept(*this);

  /* if the expression type is an integer, then that type will persist back up the call tree. So no need to return an error condition*/
  if (m_ret_type != Ret_type::INT)
  {
    std::cout << "Error, expectd an int, but instead found type " << type_to_string(m_ret_type) << std::endl;
    m_parse_flag = false;
  }
}

/*

*/
void Type_checker::dispatch(Var_dec &node) {}

/*

*/
void Type_checker::dispatch(Var_ref &node) {}

/*

*/
void Type_checker::dispatch(While_dec &node) {}

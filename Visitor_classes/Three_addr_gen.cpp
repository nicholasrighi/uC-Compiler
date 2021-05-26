// Implements
#include "Three_addr_gen.h"

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

void Three_addr_gen::print_IR_code()
{
  for (three_addr_code_entry IR_entry : m_intermediate_rep)
  {
    /*  Print out the new temporary, if one exists */
    if (std::get<0>(IR_entry).is_valid())
    {
      std::cout << std::get<0>(IR_entry).to_string() << " = ";
    }

    /*  Print out operator, which will always exist */
    std::cout << three_op_to_string(std::get<1>(IR_entry)) << " ";

    /*  Print out first temporary being used, if it exists */
    if (std::get<2>(IR_entry).is_valid())
    {
      std::cout << std::get<2>(IR_entry).to_string() << ",";
    }

    /*  Print out second temporary being used, if it exists */
    if (std::get<3>(IR_entry).is_valid())
    {
      std::cout << std::get<3>(IR_entry).to_string();
    }

    std::cout << std::endl;
  }
}

/*
  Assign symbol table and init first temp variable to 0 and first label entry to 0
*/
Three_addr_gen::Three_addr_gen(Symbol_table *sym_table) : m_temp_index(0), m_label_index(0), m_sym_table(sym_table) {}

/*
  Computes IR for access expression and then assigns the resulting array access to a new temporary
*/
void Three_addr_gen::dispatch(Array_access &node)
{
  /*  compute temporary for array access */
  node.m_access_index->accept(*this);

  /*  calculate absolute address for array access */

  /*  
      Need to multiply value of m_last_entry by 8, since all of our variables are 8 bytes wide.
  */
  Three_addr_var scaled_access_temp(gen_temp());

  three_addr_code_entry scaled_access_index = std::make_tuple(scaled_access_temp, Three_addr_OP::MULT, m_last_entry, Three_addr_var(8));

  Three_addr_var final_addr_temp(gen_temp());

  /*  calculate final address of array access */
  three_addr_code_entry dest_addr = std::make_tuple(final_addr_temp, Three_addr_OP::ADD, Three_addr_var(node.m_var->m_name), scaled_access_temp);

  m_last_entry = final_addr_temp;

  m_intermediate_rep.push_back(scaled_access_index);
  m_intermediate_rep.push_back(dest_addr);
}

/* 
  Array declerations don't require creating any temporary variables, so function doesn't do anything
*/
void Three_addr_gen::dispatch(Array_dec &node)
{
  /* do nothing */
}

/*
  Calculates expressions for each left/right sub expression and returns a new temporary using the sub expressions
  and the operator stored in binop
*/
void Three_addr_gen::dispatch(Binop_dec &node)
{
  /*  Calculate left and right sub expressions */
  node.m_left->accept(*this);
  Three_addr_var left_temp = m_last_entry;

  node.m_right->accept(*this);
  Three_addr_var right_temp = m_last_entry;

  /* Map operators that have corresponding machine instructions */
  if (node.m_op == "+")
  {
    m_last_entry = Three_addr_var(gen_temp());
    m_intermediate_rep.push_back(std::make_tuple(m_last_entry, Three_addr_OP::ADD, left_temp, right_temp));
    return;
  }
  else if (node.m_op == "-")
  {
    m_last_entry = Three_addr_var(gen_temp());
    m_intermediate_rep.push_back(std::make_tuple(m_last_entry, Three_addr_OP::SUB, left_temp, right_temp));
    return;
  }
  else if (node.m_op == "*")
  {
    m_last_entry = Three_addr_var(gen_temp());
    m_intermediate_rep.push_back(std::make_tuple(m_last_entry, Three_addr_OP::MULT, left_temp, right_temp));
    return;
  }
  else if (node.m_op == "/")
  {
    m_last_entry = Three_addr_var(gen_temp());
    m_intermediate_rep.push_back(std::make_tuple(m_last_entry, Three_addr_OP::DIVIDE, left_temp, right_temp));
    return;
  }
  /* assignements require that we load a value from the right hand side into the address of the left hand side */
  else if (node.m_op == "=")
  {
    Three_addr_var loaded_temp(gen_temp());
    /* Loads the value from the address specified by the right side of = operator into a temporary */
    m_intermediate_rep.push_back(std::make_tuple(loaded_temp, Three_addr_OP::LOAD, right_temp, Three_addr_var()));
    
    /*  Store the value of that temporary into the address specified by the left side of the = operator */
    m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::STORE, loaded_temp, left_temp));
  }
}

void Three_addr_gen::dispatch(Func_dec &node)
{
  /*  Create temporary for function body */
  node.m_func_body->accept(*this);
}

void Three_addr_gen::dispatch(Func_ref &node)
{
  //TODO.
}

void Three_addr_gen::dispatch(If_dec &node)
{
}

void Three_addr_gen::dispatch(Number &node)
{
  /* don't need a temporary variable, assign value to m_last_entry */
  m_last_entry = Three_addr_var(node.m_value);
}

void Three_addr_gen::dispatch(Return_dec &node)
{
  /* calculate temporary for body */
  node.m_return_value->accept(*this);

  /*  return that temporary */
  m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::RET, m_last_entry, Three_addr_var()));
}

void Three_addr_gen::dispatch(Stmt_dec &node)
{
  /*  generate 3 addr code for every child node */
  for (auto &sub_expr : node.m_sub_expressions)
  {
    sub_expr->accept(*this);
  }
}

void Three_addr_gen::dispatch(Unop_dec &node)
{
}

void Three_addr_gen::dispatch(Var_dec &node)
{
}

/*
  Assigns m_last_entry to the variable being referenced
*/
void Three_addr_gen::dispatch(Var_ref &node)
{
  m_last_entry = Three_addr_var(node.m_name);
}

void Three_addr_gen::dispatch(While_dec &node)
{
}

std::string three_op_to_string(Three_addr_OP op)
{
  if (op == Three_addr_OP::MOVE)
  {
    return "MOVE";
  }
  else if (op == Three_addr_OP::LOAD)
  {
    return "LOAD";
  }
  else if (op == Three_addr_OP::STORE)
  {
    return "STORE";
  }
  else if (op == Three_addr_OP::LOG_AND)
  {
    return "LOD_AND";
  }
  else if (op == Three_addr_OP::BIT_AND)
  {
    return "BIT_AND";
  }
  else if (op == Three_addr_OP::LOG_OR)
  {
    return "LOG_OR";
  }
  else if (op == Three_addr_OP::BIT_OR)
  {
    return "BIT_OR";
  }
  else if (op == Three_addr_OP::ADD)
  {
    return "ADD";
  }
  else if (op == Three_addr_OP::SUB)
  {
    return "SUB";
  }
  else if (op == Three_addr_OP::MULT)
  {
    return "MULT";
  }
  else if (op == Three_addr_OP::DIVIDE)
  {
    return "DIVIDE";
  }
  else if (op == Three_addr_OP::UMINUS)
  {
    return "UMINUS";
  }
  else if (op == Three_addr_OP::LESS_THAN)
  {
    return "LESS_THAN";
  }
  else if (op == Three_addr_OP::GREATER_THAN)
  {
    return "GREATER_THAN";
  }
  else if (op == Three_addr_OP::LOG_INVERT)
  {
    return "LOG_INVERT";
  }
  else if (op == Three_addr_OP::RET)
  {
    return "RET";
  }
  else if (op == Three_addr_OP::LABEL)
  {
    return "LABEL";
  }
  return "ERROR IN OP TO STR";
}
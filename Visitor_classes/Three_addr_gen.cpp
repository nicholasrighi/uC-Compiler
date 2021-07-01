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

/* returns the next avaliable temporary variable */
std::string Three_addr_gen::gen_temp()
{
  int stored_index = m_temp_index;
  m_temp_index++;
  return "t" + std::to_string(stored_index);
}

/* Returns the next avaliable label  */
std::string Three_addr_gen::gen_label()
{
  int stored_index = m_label_index;
  m_label_index++;
  return ".L" + std::to_string(stored_index);
}

void Three_addr_gen::print_IR_code()
{
  for (three_addr_code_entry &IR_entry : m_intermediate_rep)
  {
    /*  Print out the new temporary, if one exists */
    if (std::get<0>(IR_entry).is_valid())
    {
      m_debug_log << std::get<0>(IR_entry).to_string() << " = ";
    }

    /*  Print out operator, which will always exist */
    m_debug_log << three_op_to_string(std::get<1>(IR_entry)) << " ";

    /*  Print out first temporary being used, if it exists */
    if (std::get<2>(IR_entry).is_valid())
    {
      m_debug_log << std::get<2>(IR_entry).to_string() << ",";
    }

    /*  Print out second temporary being used, if it exists */
    if (std::get<3>(IR_entry).is_valid())
    {
      m_debug_log << std::get<3>(IR_entry).to_string();
    }

    m_debug_log << std::endl;
  }
}

/*
  Assign symbol table and init first temp variable to 0 and first label entry to 0
*/
Three_addr_gen::Three_addr_gen(std::ofstream &debug_log, Program_symbol_table &sym_table,
                               std::vector<three_addr_code_entry> &IR_code)
    : m_temp_index(0), m_label_index(0), m_sym_table(sym_table), m_intermediate_rep(IR_code),
      m_debug_log(debug_log)
{
  m_debug_log << "Start of IR code\n"
              << std::endl;
}

/*
    Computes IR code for address of array element
*/
void Three_addr_gen::dispatch(Array_access &node)
{
  /*  compute temporary for array access */
  node.m_access_index->accept(*this);

  /*  Need to multiply value of m_last_entry by 8, since all of our variables are 8 bytes wide. */
  /*
  Three_addr_var scaled_access_temp(gen_temp());
  three_addr_code_entry scaled_access_index =
      std::make_tuple(scaled_access_temp, Three_addr_OP::MULT, m_last_entry, Three_addr_var(8));

  Three_addr_var final_addr_temp(gen_temp());
  three_addr_code_entry dest_addr =
      std::make_tuple(final_addr_temp, Three_addr_OP::ADD, Three_addr_var(node.m_var->m_name, true), scaled_access_temp);

  m_last_entry = final_addr_temp;

  m_intermediate_rep.push_back(scaled_access_index);
  m_intermediate_rep.push_back(dest_addr);
  */
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
  }
  else if (node.m_op == "-")
  {
    m_last_entry = Three_addr_var(gen_temp());
    m_intermediate_rep.push_back(std::make_tuple(m_last_entry, Three_addr_OP::SUB, left_temp, right_temp));
  }
  else if (node.m_op == "*")
  {
    m_last_entry = Three_addr_var(gen_temp());
    m_intermediate_rep.push_back(std::make_tuple(m_last_entry, Three_addr_OP::MULT, left_temp, right_temp));
  }
  else if (node.m_op == "/")
  {
    m_last_entry = Three_addr_var(gen_temp());
    m_intermediate_rep.push_back(std::make_tuple(m_last_entry, Three_addr_OP::DIVIDE, left_temp, right_temp));
  }
  else if (node.m_op == "&")
  {
    m_last_entry = Three_addr_var(gen_temp());
    m_intermediate_rep.push_back(std::make_tuple(m_last_entry, Three_addr_OP::BIT_AND, left_temp, right_temp));
  }
  else if (node.m_op == "|")
  {
    m_last_entry = Three_addr_var(gen_temp());
    m_intermediate_rep.push_back(std::make_tuple(m_last_entry, Three_addr_OP::BIT_OR, left_temp, right_temp));
  }
  else if (node.m_op == "<")
  {
    m_last_entry = Three_addr_var(gen_temp());
    m_intermediate_rep.push_back(std::make_tuple(m_last_entry, Three_addr_OP::LESS_THAN, left_temp, right_temp));
  }
  else if (node.m_op == "<=")
  {
    m_last_entry = Three_addr_var(gen_temp());
    m_intermediate_rep.push_back(std::make_tuple(m_last_entry, Three_addr_OP::LESS_THAN_EQUAL, left_temp, right_temp));
  }
  else if (node.m_op == ">")
  {
    m_last_entry = Three_addr_var(gen_temp());
    m_intermediate_rep.push_back(std::make_tuple(m_last_entry, Three_addr_OP::GREATER_THAN, left_temp, right_temp));
  }
  else if (node.m_op == ">=")
  {
    m_last_entry = Three_addr_var(gen_temp());
    m_intermediate_rep.push_back(std::make_tuple(m_last_entry, Three_addr_OP::GREATER_THAN_EQUAL, left_temp, right_temp));
  }
  else if (node.m_op == "=")
  {
    // TODO. Need to check if the variable is an array. If it is an array, then need to store value to memory
    // instead of loading it into a register
    if (left_temp.is_string())
    {
      m_intermediate_rep.push_back(std::make_tuple(left_temp, Three_addr_OP::ASSIGN, right_temp, Three_addr_var()));
    }
    else
    {
      Three_addr_var new_temp = gen_temp();
      m_intermediate_rep.push_back(std::make_tuple(new_temp, Three_addr_OP::ASSIGN, right_temp, Three_addr_var()));
      m_last_entry = new_temp;
    }
  }
  else if (node.m_op == "==")
  {
    Three_addr_var new_temp = gen_temp();
    m_intermediate_rep.push_back(std::make_tuple(new_temp, Three_addr_OP::EQUALITY, left_temp, right_temp));
    m_last_entry = new_temp;
  }
  else if (node.m_op == "!=")
  {
    Three_addr_var new_temp = gen_temp();
    m_intermediate_rep.push_back(std::make_tuple(new_temp, Three_addr_OP::NOT_EQUALITY, left_temp, right_temp));
    m_last_entry = new_temp;
  }
  else if (node.m_op == "&&")
  {
    Three_addr_var either_arg_is_zero = Three_addr_var(gen_label(), Three_addr_var_type::LABEL);
    Three_addr_var neither_arg_is_zero = Three_addr_var(gen_label(), Three_addr_var_type::LABEL);
    Three_addr_var result_temp = gen_temp();

    m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::CMP, Three_addr_var(0), left_temp));
    m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::EQUAL_J, either_arg_is_zero, Three_addr_var()));
    m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::CMP, Three_addr_var(0), right_temp));
    m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::EQUAL_J, either_arg_is_zero, Three_addr_var()));
    m_intermediate_rep.push_back(std::make_tuple(result_temp, Three_addr_OP::ASSIGN, Three_addr_var(1), Three_addr_var()));
    m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::UNCOND_J, neither_arg_is_zero, Three_addr_var()));
    m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::LABEL, either_arg_is_zero, Three_addr_var()));
    m_intermediate_rep.push_back(std::make_tuple(result_temp, Three_addr_OP::ASSIGN, Three_addr_var(0), Three_addr_var()));
    m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::LABEL, neither_arg_is_zero, Three_addr_var()));
    m_last_entry = result_temp;
  }
  else if (node.m_op == "||")
  {
  }
}

void Three_addr_gen::dispatch(Func_dec &node)
{
  /*  Create temporary for function body */
  node.m_func_body->accept(*this);
}

void Three_addr_gen::dispatch(Func_ref &node)
{
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
  node.m_return_value->accept(*this);

  m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::RET, m_last_entry, Three_addr_var()));
}

void Three_addr_gen::dispatch(Stmt_dec &node)
{

  if (m_top_level_stmt_dec)
  {
    m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::LABEL, Three_addr_var("main", Three_addr_var_type::LABEL), Three_addr_var()));
    m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::RAW_STR, Three_addr_var("mov %rsp, %rbp", Three_addr_var_type::RAW_STR), Three_addr_var()));
    m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::RAW_STR, Three_addr_var("add $-8, %rbp", Three_addr_var_type::RAW_STR), Three_addr_var()));
    m_top_level_stmt_dec = false;
  }

  /*  generate 3 addr code for every child node */
  for (auto &sub_expr : node.m_sub_expressions)
  {
    sub_expr->accept(*this);
  }
}

void Three_addr_gen::dispatch(Unop_dec &node)
{
}

/*  Don't need to generate any three_addr_code for variable declerations */
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
  else if (op == Three_addr_OP::ASSIGN)
  {
    return "ASSIGN";
  }
  else if (op == Three_addr_OP::STORE)
  {
    return "STORE";
  }
  else if (op == Three_addr_OP::LOG_AND)
  {
    return "LOG_AND";
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
  else if (op == Three_addr_OP::LESS_THAN_EQUAL)
  {
    return "LESS_THAN_EQUAL";
  }
  else if (op == Three_addr_OP::GREATER_THAN)
  {
    return "GREATER_THAN";
  }
  else if (op == Three_addr_OP::GREATER_THAN_EQUAL)
  {
    return "GREATER_THAN_EQUAL";
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
  else if (op == Three_addr_OP::CMP)
  {
    return "CMP";
  }
  else if (op == Three_addr_OP::UNCOND_J)
  {
    return "J";
  }
  else if (op == Three_addr_OP::EQUAL_J)
  {
    return "JE";
  }
  else if (op == Three_addr_OP::RAW_STR)
  {
    return "RAW str: ";
  }
  return "Error, invalid op recieved";
}
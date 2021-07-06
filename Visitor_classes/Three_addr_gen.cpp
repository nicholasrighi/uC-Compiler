// Implements
#include "Three_addr_gen.h"

// System includes
#include <iostream>
#include <set>

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

void Three_addr_gen::remove_op_trailing_return()
{

  bool in_return_stmt = false;

  std::vector<three_addr_code_entry> optimized_three_addr_code;

  for (three_addr_code_entry &entry : m_intermediate_rep)
  {
    if (std::get<1>(entry) == Three_addr_OP::LABEL)
    {
      in_return_stmt = false;
    }
    if (!in_return_stmt)
    {
      optimized_three_addr_code.push_back(entry);
    }
    if (std::get<1>(entry) == Three_addr_OP::RET)
    {
      in_return_stmt = true;
    }
  }

  m_intermediate_rep = std::move(optimized_three_addr_code);
}

void Three_addr_gen::remove_unused_labels()
{

  std::set<std::string> jmp_targets;

  std::vector<three_addr_code_entry> optimized_IR_code;

  for (three_addr_code_entry &entry : m_intermediate_rep)
  {
    if (std::get<1>(entry) == Three_addr_OP::UNCOND_J || std::get<1>(entry) == Three_addr_OP::NEQUAL_J ||
        std::get<1>(entry) == Three_addr_OP::EQUAL_J)
    {
      jmp_targets.insert(std::get<2>(entry).to_string());
    }
  }

  for (three_addr_code_entry &entry : m_intermediate_rep)
  {
    /* Only add labels to the IR code if some earlier jmp instruction targets them */
    if (std::get<1>(entry) == Three_addr_OP::LABEL)
    {
      if (std::get<2>(entry).to_string() == "main" || jmp_targets.count(std::get<2>(entry).to_string()) == 1)
      {
        optimized_IR_code.push_back(entry);
      }
    }
    else
    {
      optimized_IR_code.push_back(entry);
    }
  }

  m_intermediate_rep = std::move(optimized_IR_code);
}

void Three_addr_gen::merge_adjacent_labels()
{

  std::unordered_map<std::string, std::string> replace_label_mapping;

  /*  Maps adjacent labels to the first label in the sequence */
  auto add_adjacent_labels = [this](std::string label_to_use,
                                    std::unordered_map<std::string, std::string> &str_mapping,
                                    int start_index)
  {
    for (int i = start_index; i < m_intermediate_rep.size(); i++)
    {
      if (std::get<1>(m_intermediate_rep.at(i)) == Three_addr_OP::LABEL)
      {
        str_mapping.insert({std::get<2>(m_intermediate_rep.at(i)).to_string(), label_to_use});
      }
      else
      {
        return i;
      }
    }
    return -1;
  };

  /*  generates mapping between labels that are redudant and the labels to replace them */
  for (int cur_index = 0; cur_index < m_intermediate_rep.size(); cur_index++)
  {
    if (cur_index != 0)
    {

      Three_addr_OP prev_inst = std::get<1>(m_intermediate_rep.at(cur_index - 1));
      Three_addr_OP cur_isnt = std::get<1>(m_intermediate_rep.at(cur_index));

      Three_addr_var prev_var = std::get<2>(m_intermediate_rep.at(cur_index - 1));
      Three_addr_var cur_var = std::get<2>(m_intermediate_rep.at(cur_index));

      if (cur_isnt == Three_addr_OP::LABEL && prev_inst == Three_addr_OP::LABEL)
      {
        cur_index = add_adjacent_labels(prev_var.to_string(), replace_label_mapping, cur_index);
      }
    }
  }

  std::vector<three_addr_code_entry> optimized_code;

  for (three_addr_code_entry &entry : m_intermediate_rep)
  {
    Three_addr_OP cur_inst = std::get<1>(entry);
    std::string jmp_target = std::get<2>(entry).to_string();

    /*  only add label to optimized code if it's not a subsequent label */
    if (cur_inst == Three_addr_OP::LABEL)
    {
      if (replace_label_mapping.count(jmp_target) == 0)
      {
        optimized_code.push_back(entry);
      }
    }
    /*  Replace jmp target with new label if that jmp target is a subsequent label */
    else if (cur_inst == Three_addr_OP::EQUAL_J || cur_inst == Three_addr_OP::UNCOND_J || cur_inst == Three_addr_OP::NEQUAL_J)
    {
      if (replace_label_mapping.count(jmp_target) == 1)
      {
        std::get<2>(entry) = Three_addr_var(replace_label_mapping.at(jmp_target), Three_addr_var_type::LABEL);
      }
      optimized_code.push_back(entry);
    }
    else
    {
      optimized_code.push_back(entry);
    }
  }
  m_intermediate_rep = std::move(optimized_code);
}

void Three_addr_gen::print_IR_code()
{
  for (int i = 0; i < m_intermediate_rep.size(); i++)
  {

    three_addr_code_entry &IR_entry = m_intermediate_rep.at(i);

    m_debug_log << std::to_string(i) << ": ";

    if (std::get<0>(IR_entry).is_valid())
    {
      m_debug_log << std::get<0>(IR_entry).to_string() << " = ";
    }

    m_debug_log << three_op_to_string(std::get<1>(IR_entry)) << " ";

    if (std::get<2>(IR_entry).is_valid())
    {
      m_debug_log << std::get<2>(IR_entry).to_string() << ",";
    }

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
    Computes IR code for address of array element, DOES NOT load the value at the access address into memory.
    This is because expressions of the form a[x] = a[y] + z require a[y] to be loaded into memory but requires
    a[x] to be stored to memory. So the parent node needs to determine if we should load from or store to the 
    address that this dispatch method calculates
*/
void Three_addr_gen::dispatch(Array_access &node)
{
  /*  compute temporary for array access */
  node.m_access_index->accept(*this);

  /*  Multiply value of m_last_entry by 8, since all of our variables are 8 bytes wide. */
  Three_addr_var scaled_access_temp(gen_temp());
  three_addr_code_entry scaled_access_inst =
      std::make_tuple(scaled_access_temp, Three_addr_OP::MULT, m_last_entry, Three_addr_var(8));

  /*  Make address negative to handle offsets correctly */
  Three_addr_var neg_access_temp(gen_temp());
  three_addr_code_entry neg_access_inst =
      std::make_tuple(neg_access_temp, Three_addr_OP::SUB, Three_addr_var(0), scaled_access_temp);

  Three_addr_var final_addr_temp(gen_temp());
  three_addr_code_entry final_addr_inst =
      std::make_tuple(final_addr_temp, Three_addr_OP::ADD, Three_addr_var(node.m_var->m_name, Three_addr_var_type::ARRAY), neg_access_temp);

  Three_addr_var loaded_value_temp(gen_temp());
  three_addr_code_entry loaded_value_inst =
      std::make_tuple(loaded_value_temp, Three_addr_OP::LOAD, final_addr_temp, Three_addr_var());

  m_last_entry = loaded_value_temp;

  m_intermediate_rep.push_back(scaled_access_inst);
  m_intermediate_rep.push_back(neg_access_inst);
  m_intermediate_rep.push_back(final_addr_inst);
  m_intermediate_rep.push_back(loaded_value_inst);

  m_child_is_array_dec = true;
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
  node.m_right->accept(*this);
  Three_addr_var right_temp = m_last_entry;
  bool right_child_is_array = m_child_is_array_dec;
  m_child_is_array_dec = false;

  /*  
      Need to visit left side second to ensure that the '=' operator functions correctly when the left side is 
      an array
  */
  node.m_left->accept(*this);
  Three_addr_var left_temp = m_last_entry;
  bool left_child_is_array = m_child_is_array_dec;
  m_child_is_array_dec = false;

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
    /*  
        If left child is an array, then we don't need to load the value from the array access into memory, which is what 
        the Array_access visitor node does by default. Instead we need to store the right hand side to the array access
        addr
    */
    if (left_child_is_array)
    {
      m_intermediate_rep.pop_back();
      Three_addr_var &store_addr_tmp = std::get<0>(m_intermediate_rep.back());
      m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::STORE, store_addr_tmp, right_temp));
    }
    else
    {
      m_intermediate_rep.push_back(std::make_tuple(left_temp, Three_addr_OP::ASSIGN, right_temp, Three_addr_var()));
    }
    m_last_entry = left_temp;
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
    Three_addr_var either_arg_is_one = Three_addr_var(gen_label(), Three_addr_var_type::LABEL);
    Three_addr_var neither_arg_is_one = Three_addr_var(gen_label(), Three_addr_var_type::LABEL);
    Three_addr_var return_label = Three_addr_var(gen_label(), Three_addr_var_type::LABEL);
    Three_addr_var result_temp = gen_temp();

    m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::CMP, Three_addr_var(0), left_temp));
    m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::NEQUAL_J, either_arg_is_one, Three_addr_var()));
    m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::CMP, Three_addr_var(0), right_temp));
    m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::EQUAL_J, neither_arg_is_one, Three_addr_var()));
    m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::LABEL, either_arg_is_one, Three_addr_var()));
    m_intermediate_rep.push_back(std::make_tuple(result_temp, Three_addr_OP::ASSIGN, Three_addr_var(1), Three_addr_var()));
    m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::UNCOND_J, return_label, Three_addr_var()));
    m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::LABEL, neither_arg_is_one, Three_addr_var()));
    m_intermediate_rep.push_back(std::make_tuple(result_temp, Three_addr_OP::ASSIGN, Three_addr_var(0), Three_addr_var()));
    m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::LABEL, return_label, Three_addr_var()));
    m_last_entry = result_temp;
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
  node.m_cond->accept(*this);

  Three_addr_var if_false_label = Three_addr_var(gen_label(), Three_addr_var_type::LABEL);
  Three_addr_var end_of_if_label = Three_addr_var(gen_label(), Three_addr_var_type::LABEL);

  /* Drop through condition is taken if condition evaluates to 1, otherwise jump to false part of if/else statement */
  m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::CMP, Three_addr_var(1), m_last_entry));
  m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::NEQUAL_J, if_false_label, Three_addr_var()));

  /* Insert instructions corresponding to condition being true */
  if (node.m_stmt_if_true != nullptr)
  {
    node.m_stmt_if_true->accept(*this);
  }

  m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::UNCOND_J, end_of_if_label, Three_addr_var()));
  m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::LABEL, if_false_label, Three_addr_var()));

  /* Insert instructions corresponding to condition being false */
  if (node.m_stmt_if_false != nullptr)
  {
    node.m_stmt_if_false->accept(*this);
  }

  m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::LABEL, end_of_if_label, Three_addr_var()));
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

  bool saved_top_level = m_top_level_stmt_dec;

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

  /*  remove any instructions that can't be reached from m_intermediate_rep */
  if (saved_top_level)
  {
    remove_op_trailing_return();
    remove_unused_labels();
    merge_adjacent_labels();
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
  Three_addr_var body_label(gen_label(), Three_addr_var_type::LABEL);
  Three_addr_var condition_label(gen_label(), Three_addr_var_type::LABEL);

  m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::UNCOND_J, condition_label, Three_addr_var()));
  m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::LABEL, body_label, Three_addr_var()));

  if (node.m_body != nullptr)
  {
    node.m_body->accept(*this);
  }

  m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::LABEL, condition_label, Three_addr_var()));

  node.m_cond->accept(*this);

  m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::CMP, Three_addr_var(1), m_last_entry));
  m_intermediate_rep.push_back(std::make_tuple(Three_addr_var(), Three_addr_OP::EQUAL_J, body_label, Three_addr_var()));
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
  else if (op == Three_addr_OP::NEQUAL_J)
  {
    return "JNE";
  }
  else if (op == Three_addr_OP::RAW_STR)
  {
    return "RAW str: ";
  }
  else if (op == Three_addr_OP::EQUALITY)
  {
    return "EQUAL ";
  }
  else if (op == Three_addr_OP::NOT_EQUALITY)
  {
    return "NOT EQUAL ";
  }
  return "Error, invalid op recieved";
}
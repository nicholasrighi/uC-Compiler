/*  System includes */
#include <iostream>

/*  Implements */
#include "Program_symbol_table.h"

Program_symbol_table::Program_symbol_table() : m_cur_func_iter(m_function_sym_table.end())
{
}

std::optional<Var_dec *> Program_symbol_table::get_var_dec(std::string var_name)
{

  /*  No current function specified, only search global symbol table */
  if (m_cur_func_iter == m_function_sym_table.end())
  {
    return m_global_sym_table.get_var_dec(var_name);
  }

  /*  Return var dec for current function table if that variable is declared inside that function */
  if (m_cur_func_iter->second.get_var_dec(var_name))
  {
    return m_cur_func_iter->second.get_var_dec(var_name);
  }

  /*  Otherwise return the global var dec */
  return m_global_sym_table.get_var_dec(var_name);
}

bool Program_symbol_table::add_global_var(std::string name, sym_table_entry sym_entry)
{
  return m_global_sym_table.add_var(name, sym_entry);
}

bool Program_symbol_table::add_local_var(std::string name, sym_table_entry sym_entry)
{
  if (m_cur_func_iter == m_function_sym_table.end())
  {
    return false;
  }

  return m_cur_func_iter->second.add_var(name, sym_entry);
}

void Program_symbol_table::set_search_func(std::string func_name)
{
  m_cur_func_iter = m_function_sym_table.find(func_name);
}

bool Program_symbol_table::add_function(std::string func_name) {

  if (m_function_sym_table.insert({func_name, Function_symbol_table{}}).second) {
    m_cur_func_iter = m_function_sym_table.find(func_name);
    return true;
  }
  return false;
}

std::optional<Var_dec*> Program_symbol_table::get_func_dec(std::string func_name) {
  return m_global_sym_table.get_var_dec(func_name);
}

void Program_symbol_table::reset_sym_table_scopes() {
  m_global_sym_table.reset_scope();
  for (auto& table :  m_function_sym_table) {
    table.second.reset_scope();
  }
}

void Program_symbol_table::print_prog_sym_table() {
  std::cout << "global variables are" << std::endl;
  m_global_sym_table.print_sym_table();

  for (auto& table : m_function_sym_table) {
    std::cout << "Function " << table.first << " has symbol table " << std::endl;
    table.second.print_sym_table();
  }
}
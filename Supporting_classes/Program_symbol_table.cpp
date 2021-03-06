/*  System includes */
#include <iostream>

/*  Implements */
#include "Program_symbol_table.h"

Program_symbol_table::Program_symbol_table(std::ofstream &debug_file)
    : m_debug_file(debug_file), m_cur_func_iter(m_function_sym_table.end()), m_global_sym_table(debug_file)
{
}

std::optional<Var_dec *> Program_symbol_table::get_var_dec(const std::string &var_name)
{
  std::optional<sym_table_entry> entry = get_sym_table_entry(var_name);
  if (entry.has_value())
  {
    return std::get<0>(entry.value());
  }
  return std::nullopt;
}

std::optional<Var_storage> Program_symbol_table::get_var_storage(const std::string &var_name)
{
  std::optional<sym_table_entry> entry = get_sym_table_entry(var_name);
  if (entry.has_value())
  {
    return std::get<1>(entry.value());
  }
  return std::nullopt;
}

std::optional<int> Program_symbol_table::get_var_offset(const std::string &var_name)
{
  std::optional<sym_table_entry> entry = get_sym_table_entry(var_name);
  if (entry.has_value())
  {
    return std::get<2>(entry.value());
  }
  return std::nullopt;
}

std::optional<sym_table_entry> Program_symbol_table::get_sym_table_entry(const std::string &name)
{
  /*  No current function specified, only search global symbol table */
  if (m_cur_func_iter == m_function_sym_table.end())
  {
    return m_global_sym_table.get_sym_table_entry(name);
  }

  /*  
      Return var offset for current function table if that variable is declared 
      inside that function 
  */
  if (m_cur_func_iter->second.get_sym_table_entry(name))
  {
    return m_cur_func_iter->second.get_sym_table_entry(name);
  }

  /*  Otherwise return the global var dec */
  return m_global_sym_table.get_sym_table_entry(name);
}

int Program_symbol_table::get_cur_func_rsp_offset()
{
  return m_cur_func_iter->second.get_rsp_offset();
}

bool Program_symbol_table::add_global_var(std::string name, Var_dec *var_dec)
{
  return m_global_sym_table.add_var(name, var_dec, Var_storage::GLOBAL);
}

bool Program_symbol_table::add_local_var(std::string name, Var_dec *var_dec)
{
  if (m_cur_func_iter == m_function_sym_table.end())
  {
    return false;
  }

  return m_cur_func_iter->second.add_var(name, var_dec, Var_storage::LOCAL);
}

bool Program_symbol_table::add_register_var(std::string name, Var_dec *var_dec)
{
  if (m_cur_func_iter == m_function_sym_table.end())
  {
    return false;
  }

  return m_cur_func_iter->second.add_var(name, var_dec, Var_storage::REGISTER);
}

void Program_symbol_table::set_search_func(std::string func_name)
{
  m_cur_func_iter = m_function_sym_table.find(func_name);
}

bool Program_symbol_table::add_function(std::string func_name)
{

  /*  Insert returns <iterator, bool>, where bool determines if insert succeded */
  if (m_function_sym_table.insert({func_name, Function_symbol_table(m_debug_file)}).second)
  {
    m_cur_func_iter = m_function_sym_table.find(func_name);
    return true;
  }
  return false;
}

std::optional<Var_dec *> Program_symbol_table::get_func_dec(std::string func_name)
{
  return m_global_sym_table.get_var_dec(func_name);
}

void Program_symbol_table::reset_sym_table_scopes()
{
  m_global_sym_table.reset_scope();
  for (auto &table : m_function_sym_table)
  {
    table.second.reset_scope();
  }
}

void Program_symbol_table::add_scope_to_cur_func_sym_table()
{
  m_cur_func_iter->second.add_scope();
}

bool Program_symbol_table::increment_scope_of_cur_fun_sym_table()
{
  if (m_cur_func_iter == m_function_sym_table.end())
  {
    return false;
  }
  return m_cur_func_iter->second.increment_scope();
}

void Program_symbol_table::print_prog_sym_table()
{
  m_debug_file << "global variables are" << std::endl;
  m_global_sym_table.print_sym_table();

  for (auto &table : m_function_sym_table)
  {
    m_debug_file << "Function " << table.first << " has symbol table " << std::endl;
    table.second.print_sym_table();
  }
  m_debug_file << std::endl;
}

std::vector<std::string> Program_symbol_table::get_cur_func_args()
{
  if (m_cur_func_iter != m_function_sym_table.end())
  {
    return m_cur_func_iter->second.get_func_arg_names();
  }
  return std::vector<std::string>();
}
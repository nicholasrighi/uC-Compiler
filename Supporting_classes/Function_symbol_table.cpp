// Implements
#include "Function_symbol_table.h"
#include "../AST_classes/Array_dec.h"

// System includes
#include <iostream>
#include <algorithm>

Function_symbol_table::Function_symbol_table(std::ofstream &debug_file) : m_debug_file(debug_file)
{
  m_chained_sym_table.push_back(std::unordered_map<std::string, sym_table_entry>{});
}

bool Function_symbol_table::add_var(std::string name, Var_dec *var_dec, Var_storage var_storage)
{
  sym_table_entry sym_entry(var_dec, var_storage, m_local_var_offset);

  /*  Check that variable wasn't already declared in the current scope */
  if (m_chained_sym_table.at(m_table_level).count(name) >= 1 || m_func_arg_table.count(name) >= 1)
  {
    return false;
  }

  if (var_storage == Var_storage::REGISTER)
  {
    m_func_arg_table.insert({name, sym_entry});
    m_func_arg_vec.insert(m_func_arg_vec.begin(), name);
  }
  else
  {
    m_chained_sym_table.at(m_table_level).insert({name, sym_entry});
  }

  /*  If an array is declared in a function argument, we only need to store its address as a local variable */
  if (var_dec != nullptr && var_dec->m_obj_type == Object_type::ARRAY && var_storage != Var_storage::REGISTER)
  {
    m_local_var_offset -= ((dynamic_cast<Array_dec *>(var_dec))->m_array_size) * 8;
  }
  else
  {
    m_local_var_offset -= 8;
  }
  return true;
}

std::optional<Var_dec *> Function_symbol_table::get_var_dec(const std::string &name)
{
  std::optional<sym_table_entry> entry = get_sym_table_entry(name);
  if (entry.has_value())
  {
    return std::get<0>(entry.value());
  }
  return std::nullopt;
}

std::optional<Var_storage> Function_symbol_table::get_var_storage(const std::string &name)
{
  std::optional<sym_table_entry> entry = get_sym_table_entry(name);
  if (entry.has_value())
  {
    return std::get<1>(entry.value());
  }
  return std::nullopt;
}

std::optional<int> Function_symbol_table::get_var_offset(const std::string &name)
{
  std::optional<sym_table_entry> entry = get_sym_table_entry(name);
  if (entry.has_value())
  {
    return std::get<2>(entry.value());
  }
  return std::nullopt;
}

std::optional<sym_table_entry> Function_symbol_table::get_sym_table_entry(const std::string &name)
{
  /* only start checking at the current scope, not the most nested scope */
  for (int cur_index = m_table_level; cur_index >= 0; cur_index--)
  {
    auto &table = m_chained_sym_table.at(cur_index);
    if (table.count(name) == 1)
    {
      return table.at(name);
    }
  }

  if (m_func_arg_table.count(name) >= 1)
  {
    return m_func_arg_table.at(name);
  }

  return std::nullopt;
}

int Function_symbol_table::get_rsp_offset()
{
  return m_local_var_offset + 8;
}

void Function_symbol_table::print_sym_table()
{
  for (auto &tables : m_chained_sym_table)
  {
    for (auto &symbols : tables)
    {
      int offset = std::get<2>(symbols.second);
      m_debug_file << "\tVariable " << symbols.first << " is defined at offset "
                   << std::to_string(offset) << std::endl;
    }
  }
}

/*  Most nested scope is the back of the vector, so increment m_table_level */
void Function_symbol_table::add_scope()
{
  m_chained_sym_table.push_back(std::unordered_map<std::string, sym_table_entry>{});
  m_table_level++;
}

bool Function_symbol_table::increment_scope()
{
  m_table_level++;
  return !(m_table_level >= m_chained_sym_table.size());
}

void Function_symbol_table::reset_scope()
{
  m_table_level = 0;
}

std::vector<std::string> Function_symbol_table::get_func_arg_names()
{
  return m_func_arg_vec;
}
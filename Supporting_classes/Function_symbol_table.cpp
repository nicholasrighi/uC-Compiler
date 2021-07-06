// Implements
#include "Function_symbol_table.h"
#include "../AST_classes/Array_dec.h"

// System includes
#include <iostream>

Function_symbol_table::Function_symbol_table()
{
  m_chained_sym_table.push_back(std::unordered_map<std::string, sym_table_entry>{});
}

bool Function_symbol_table::add_var(std::string name, Var_dec *var_dec, Var_storage var_storage)
{
  sym_table_entry sym_entry(var_dec, var_storage, m_local_var_offset);
  {
    if (m_chained_sym_table.at(m_table_level).count(name) >= 1)
    {
      return false;
    }
    m_chained_sym_table.at(m_table_level).insert({name, sym_entry});
    if (var_dec != nullptr && var_dec->m_obj_type == Object_type::ARRAY)
    {
      m_local_var_offset += ((dynamic_cast<Array_dec *>(var_dec))->m_array_size) * 8;
    }
    else
    {
      m_local_var_offset += 8;
    }
    return true;
  }
}

std::optional<Var_dec *> Function_symbol_table::get_var_dec(std::string name)
{
  /* only start checking at the currently recorded scope, not the most nested scope */
  for (int cur_index = m_table_level; cur_index >= 0; cur_index--)
  {
    auto &table = m_chained_sym_table.at(cur_index);
    if (table.count(name) == 1)
    {
      return {std::get<0>(table.at(name))};
    }
  }
  return std::nullopt;
}

std::optional<int> Function_symbol_table::get_var_offset(std::string name)
{
  /* only start checking at the current scope, not the most nested scope */
  for (int cur_index = m_table_level; cur_index >= 0; cur_index--)
  {
    auto &table = m_chained_sym_table.at(cur_index);
    if (table.count(name) == 1)
    {
      return {std::get<2>(table.at(name))};
    }
  }
  return std::nullopt;
}

void Function_symbol_table::print_sym_table()
{
  for (auto &tables : m_chained_sym_table)
  {
    for (auto &symbols : tables)
    {
      std::cout << "Variable " << symbols.first << " is defined" << std::endl;
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
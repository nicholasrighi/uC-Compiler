// Implements
#include "Function_symbol_table.h"

// System includes
#include <iostream>

Function_symbol_table::Function_symbol_table()
{
  m_chained_sym_table.push_back(std::unordered_map<std::string, sym_table_entry>{});
  m_table_level = 0; 
}

bool Function_symbol_table::add_var(std::string name, Var_dec* var_dec, Var_storage var_storage)
{
  sym_table_entry sym_entry(var_dec, var_storage, m_local_var_offset);
  {
    if (m_chained_sym_table.front().count(name) >= 1)
    {
      return false;
    }
    m_chained_sym_table.front().insert({name, sym_entry});
    m_local_var_offset += 8;
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
  /* if we exit the loop, variable isn't in table, return false in optional */
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

void Function_symbol_table::add_scope()
{
  m_chained_sym_table.push_back(std::unordered_map<std::string, sym_table_entry>{});
  /*  need the iterator to point to the new most nested scope */
  m_table_level++;
}

void Function_symbol_table::increase_scope_depth()
{
  m_table_level--;
}

void Function_symbol_table::decrease_scope_depth()
{
  /*  
      doesn't remove the most nested symbol table from the list, only ensures that the more nested scope(s) 
      won't be checked when get_var_dec() is called
  */
  m_table_level++;
}

void Function_symbol_table::reset_scope() {
  m_table_level = 0;
}
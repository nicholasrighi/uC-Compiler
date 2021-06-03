// Implements
#include "Symbol_table.h"

// System includes
#include <iostream>

Symbol_table::Symbol_table()
{
  m_chained_sym_table.push_back(std::unordered_map<std::string, sym_table_entry>{});
  m_table_level = m_chained_sym_table.begin();
}

bool Symbol_table::add_var(std::string name, sym_table_entry sym_entry)
{
  {
    if (m_chained_sym_table.front().count(name) >= 1)
    {
      return false;
    }
    m_chained_sym_table.front().insert({name, sym_entry});
    return true;
  }
}

std::optional<Var_dec *> Symbol_table::get_var_dec(std::string name)
{
  /* only start checking at the currently recorded scope, not the most nested scope */
  for (; m_table_level != m_chained_sym_table.end(); m_table_level++)
  {
    auto &table = *m_table_level;
    if (table.count(name) == 1)
    {
      return {std::get<0>(table.at(name))};
    }
  }
  /* if we exit the loop, variable isn't in table, return false in optional */
  return std::nullopt;
}

void Symbol_table::print_sym_table()
{
  for (auto &tables : m_chained_sym_table)
  {
    for (auto &symbols : tables)
    {
      std::cout << "Variable " << symbols.first << " is defined" << std::endl;
    }
  }
}

void Symbol_table::add_level()
{
  m_chained_sym_table.push_front(std::unordered_map<std::string, sym_table_entry>{});
  /*  need the iterator to point to the new most nested scope */
  m_table_level--;
}

void Symbol_table::remove_level()
{
  /*  doesn't remove the most nested symbol table from the list, only ensures that the more nested scope(s) 
      won't be checked when get_var_dec() is called
  */
  m_table_level++;
}
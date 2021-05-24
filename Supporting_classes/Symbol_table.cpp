// Implements
#include "Symbol_table.h"

// System includes
#include <iostream>

bool Symbol_table::add_var(std::string name, Var_dec *variable)
{
  {
    /*
      Adds a variable to the top level of the symbol table if the variable isn't already 
      declared at the top level. Returns false is the variable is already declared in the 
      top level of the symbol table, otherwise returns true
    */
    if (chained_sym_table.front().count(name) >= 1)
    {
      return false;
    }
    chained_sym_table.front().insert({name, variable});
    return true;
  }
}

std::optional<Var_dec *> Symbol_table::get_var_dec(std::string name)
{
  /* iterate through all tables, starting at the most nested scope (ie. front of the table) */
  for (auto &table : chained_sym_table)
  {
    /* if we find the variable we're looking for, return it */
    if (table.count(name) == 1)
    {
      return {table.at(name)};
    }
  }
  /* if we exit the loop, variable isn't in table, return false in optional */
  return std::nullopt;
}

void Symbol_table::print_sym_table()
{
  for (auto &tables : chained_sym_table)
  {
    for (auto &symbols : tables)
    {
      std::cout << "Variable " << symbols.first << " is defined" << std::endl;
    }
  }
}


void Symbol_table::add_level() {
  chained_sym_table.push_front(std::unordered_map<std::string, Var_dec*> {});
}

void Symbol_table::remove_level() {
  chained_sym_table.pop_front();
}
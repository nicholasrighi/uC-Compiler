#include <iostream>
#include "Symbol_table.h"

bool Symbol_table::add_var(std::string name, Var_dec* variable)
{
  {
    // redefining a variable isn't allowed, so we need to check if it's already in the
    // table before we add it. Return an error if the variable is already in the table
    if (sym_table.count(name) >= 1)
    {
      return false;
    }
    sym_table.insert({name, variable});
    return true;
  }
}

std::optional<Var_dec*> Symbol_table::get_var_dec(std::string name)
{
  // return false if the variable we're searching for isn't in the symbol table.
  if (sym_table.count(name) != 1)
  {
    return std::nullopt;
  }
  // otherwise return the variable we're looking for 
  return {sym_table.at(name)};
}

void Symbol_table::print_sym_table()
{
  for (auto &symbols : sym_table)
  {
    std::cout << "Variable " << symbols.first << " is defined" << std::endl;
  }
}
#pragma once

#include <unordered_map>
#include <string>
#include <optional>
#include "Var_dec.h"

#define ERROR -1

class Symbol_table
{
public:
  // Adds a variable of the specified type to the symbol table. Returns ERROR if
  // the variable was already defined. Returns 0 otherwise
  bool add_var(std::string name, Var_dec* variable);

  // Returns an optional containing the type of the specified variable. If the
  // variable isn't defined, then the returned optional is false.
  std::optional<Var_dec*> get_var_dec(std::string name);

  // prints all symbols defined in the symbol table. Used for debugging only
  void print_sym_table();

private:
  std::unordered_map<std::string, Var_dec*> sym_table;
};
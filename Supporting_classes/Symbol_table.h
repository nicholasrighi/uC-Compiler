#pragma once

// system includes
#include <unordered_map>
#include <string>
#include <optional>
#include <list>

// local includes
#include "../AST_classes/Var_dec.h"

/* 
   A symbol table entry is composed of the variable declaration, the 
   variable's storage location in memory, and the variable's offset from the base pointer
   (the offset is only used if the variable is local, and thus on the stack)
*/
using sym_table_entry = std::tuple<Var_dec*, Var_storage, int>;

class Symbol_table
{
public:
  Symbol_table();

  /* 
     Adds a variable of the specified type to the symbol table. Returns false if
     the variable was already defined. Returns true otherwise 
  */
  bool add_var(std::string name, sym_table_entry sym_entry);

  /* 
     Returns an optional containing the type of the specified variable. If the
     variable isn't defined, then the returned optional is false 
  */
  std::optional<Var_dec *> get_var_dec(std::string name);


  /* prints all symbols defined in the symbol table. Used for debugging only */
  void print_sym_table();

  /* 
     adds a level to the symbol table. If a variable is declared in multiple levels, the 
     variable in the highest level is returned when add_var() is called
  */
  void add_level();

  /*
     Removes a level from the symbol table.
  */
  void remove_level();

  /*
    List of pointers to maps of <string, var dec>. The front of the list is the most nested
    scope, the back of the list is the least nested. Searching for a variable starts at the 
    front (most nested) and goes to the back
  */
  std::list<std::unordered_map<std::string, sym_table_entry>> chained_sym_table;
};
#pragma once

// system includes
#include <unordered_map>
#include <string>
#include <optional>
#include <list>

// local includes
#include "../AST_classes/Var_dec.h"
#include "../AST_classes/Var_type_decs.h"

/* 
   A symbol table entry is composed of the variable declaration, the 
   variable's storage location in memory, and the variable's offset from the base pointer
   (the offset is only used if the variable is local, and thus on the stack)
*/
using sym_table_entry = std::tuple<Var_dec *, Var_storage, int>;

class Symbol_table
{
public:
   Symbol_table();

   /* 
     Adds a variable of the specified type to the most nested symbol table. Returns false if
     the variable was already defined. Returns true otherwise 
  */
   bool add_var(std::string name, sym_table_entry sym_entry);

   /* 
     Returns an optional containing the variable decleration of the specified variable. If the
     variable isn't defined, then the returned optional is false 
  */
   std::optional<Var_dec *> get_var_dec(std::string name);

   /* prints all symbols defined in the symbol table. Used for debugging only */
   void print_sym_table();

   /* 
     adds a level to the symbol table by adding a new symbol table to the front of the list. If a variable is 
     declared in multiple scopes, the most nested scope of that variable is returned when add_var() is called
  */
   void add_level();

   /*
     Removes a level from the symbol table. This doesn't remove that symbol table from the list, but rather
     increments the m_table_level iterator by 1. This ensures that the more nested scope is still avaliable to 
     other visitors without needing to reconstruct the entire symbol table for a second time.
  */
   void remove_level();

   /*
      Stores a list of maps that associate a variable name with a symbol table entry. Front of 
      the list is the most nested scope, back of the list is the least nested scope
   */
   std::list<std::unordered_map<std::string, sym_table_entry>> m_chained_sym_table;

   /*
      The iterator into the symbol table representing the most nested scope being evaluated. 
      This is used by visitors to allow them to go into more nested scopes of the symbol table
      as they traverse the AST
   */
   std::list<std::unordered_map<std::string, sym_table_entry>>::iterator m_table_level;
};
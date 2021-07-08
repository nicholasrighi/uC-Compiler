#pragma once

// system includes
#include <unordered_map>
#include <string>
#include <optional>
#include <vector>

// local includes
#include "../AST_classes/Var_dec.h"
#include "../AST_classes/Var_type_decs.h"

/* 
   Symbol table entry is 
      Var_dec* =    pointer to var_dec node
      Var_storage = determines if variable is global or local storage
      int =         variable's offset from the base pointer of the current stack frame.
                    only relevant if the variable is LOCAL, not used if GLOBAL
*/
using sym_table_entry = std::tuple<Var_dec *, Var_storage, int>;

class Function_symbol_table
{
public:
   Function_symbol_table();

   /* 
     Adds a variable of the specified type to the most nested symbol table. Returns false if
     the variable was already defined. Returns true otherwise 
   */
   bool add_var(std::string name, Var_dec* var_dec, Var_storage var_storage);

   /* 
     Adds a variable of the specified type to the most nested symbol table with the specified offset. 
     Returns false if the variable was already defined. Returns true otherwise. Does not change the 
     m_local_var_offset, so the offset specified should be negative (ie. above %rsp) in order to not
     effect future calls to add_var().
   */
   bool add_var(std::string name, Var_dec* var_dec, Var_storage var_storage, int offset);

   /* 
     Returns an optional containing the variable decleration of the specified variable. If the
     variable isn't defined, then the returned optional is false 
   */
   std::optional<Var_dec *> get_var_dec(std::string name);

   /*
      Returns an optional containing the variable's offset from the base pointer. If the 
      variable isn't in the symbol table, then the returned optional contains nothing
   */
   std::optional<int> get_var_offset(std::string name);

   /* prints all symbols defined in the symbol table. Used for debugging only */
   void print_sym_table();

   /* 
     adds a symbol table represnting a new, inner scope to the front of the symbol table list. 
     If a variable is declared in multiple scopes, the most nested scope of that variable is 
     returned when add_var() is called
   */
   void add_scope();

   /*
      Sets m_table_level to the next most nested symbol table. If m_table_level is an invalid
      index into m_chained_symbol_table as a result of incrementing m_table_level, returns false.
      Otherwise returns true
   */
   bool increment_scope();

   /*
      Moves m_table_level back to the least nested scope of the symbol table
   */
   void reset_scope();

private:
   /*
      Stores a vector of maps that associate a variable name with a symbol table entry. Front of 
      the list is the least nested scope, back of the list is the most nested scope
   */
   std::vector<std::unordered_map<std::string, sym_table_entry>> m_chained_sym_table;

   /*
      Index into the symbol table representing the most nested scope being evaluated. 
      This is used by visitors to allow them to go into more nested scopes of the symbol table
      as they traverse the AST
   */
   int m_table_level = 0;

   /*    
      The offset that the next local variable will be given when added to the symbol table 
      This value isn't used if the function_symbol_table is holding global variables
   */
   int m_local_var_offset = 0;
};
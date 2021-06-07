#pragma once

/*  System includes */
#include <unordered_map>

/*  Local references */
#include "Symbol_table.h"

class Program_symbol_table
{
public:
    Program_symbol_table();

    /*  
      Checks the symbol table pointed to by m_cur_func_iter for the specified variable. If that
      symbol table doesn't contain the search target, checks the global symbol table for the 
      search target. Returns a pointer to Var_dec associated with that name if that variable is 
      found, otherwise returns nullopt 
    */
    std::optional<Var_dec *> get_var_dec(std::string var_name);

    /*
      Adds the mapping from var_name to sym_entry into the global symbol table.
      Returns true if a variable with var_name hasn't been declared in the most nested scope, 
      and false if a variable with var_name has been declared in the most nested scope
    */
    bool add_global_var(std::string var_name, sym_table_entry sym_entry);

    /*  
      Adds the mapping from var_name to sym_entry into the Symbol table specified by m_cur_func_iter.
      If m_cur_func_iter isn't valid, returns true. Otherwise returns true if a variable with var_name
      hasn't been declared in the most nested scope, and false if a variable with var_name has been 
      declared in the most nested scope
    */
    bool add_local_var(std::string var_name, sym_table_entry sym_entry);

    /*  
      Sets m_cur_func_iter to the entry in m_function_sym_table with the specified func_name. If
      func_name isn't in m_function_sym_table, sets m_cur_func_iter to m_function_sym_table.end(),
      which will cause future add_function_var to fail
    */
    void set_search_func(std::string func_name);

    /*  
      Adds a new symbol table for function func_name. Points m_cur_func_iter to the newly inserted
      function entry if an entry with that name doesn't exist already. Returns true if a function with
      that name doesn't exist, false otherwise.
    */
    bool add_function(std::string func_name);

    /*
      Returns an optional containing the function decleration specified by func_name
    */
    std::optional<Var_dec*> get_func_dec(std::string func_name);

    /*  Resets the scope of every symbol table to the least nested scope */
    void reset_sym_table_scopes();

    /*  Points to the function whose symbol table will be examined when get_var_dec is called */
    std::unordered_map<std::string, Symbol_table>::iterator m_cur_func_iter;

    /*  Prints every symbol table */
    void print_prog_sym_table();

private:
    /*  Stores all global variable declerations */
    Symbol_table m_global_sym_table;
    /*  Stores symbol tables for functions, mapping function name to function symbol table */
    std::unordered_map<std::string, Symbol_table> m_function_sym_table;
};
#pragma once

/*  System includes */
#include <unordered_map>
#include <fstream>

/*  Local references */
#include "Function_symbol_table.h"

class Program_symbol_table
{
public:
    Program_symbol_table(std::ofstream &debug_file);

    /*  
      Checks the symbol table pointed to by m_cur_func_iter for the specified variable. If that
      symbol table doesn't contain the search target, checks the global symbol table for the 
      search target. Returns a pointer to Var_dec associated with that name if that variable is 
      found, otherwise returns nullopt 
  */
    std::optional<Var_dec *> get_var_dec(const std::string& var_name);

    /*
      Returns the offset of the variable associated with var_name, if that
      variable has been entered into the symbol table. Otherwise returns
      an empty optional
    */
    std::optional<int> get_var_offset(const std::string& var_name);

    /*  
        Returns an optional containing the storage location of the specified variable in the symbol table
        pointed to by m_cur_func_iter. If the specified variable isn't found in that symbol table, then
        it returns an empty optional.
    */
    std::optional<Var_storage> get_var_storage(const std::string& var_name);

    /*    
        Returns the offest to subtract from the stack pointer based on the offsets of the local variables
        stored in the symbol table currently pointed to by m_cur_func_iter
  */
    int get_cur_func_rsp_offset();

    /*
      Adds the mapping from var_name to sym_entry into the global symbol table.
      Returns true if a variable with var_name hasn't been declared in the most nested scope, 
      and false if a variable with var_name has been declared in the most nested scope
  */
    bool add_global_var(std::string var_name, Var_dec *var_dec);

    /*  
      Adds the mapping from var_name to sym_entry into the Symbol table specified by m_cur_func_iter.
      Variable is added as a local variables. If m_cur_func_iter isn't valid, returns true. 
      Otherwise returns true if a variable with var_name hasn't been declared in the most nested scope, 
      and false if a variable with var_name has been declared in the most nested scope
  */
    bool add_local_var(std::string var_name, Var_dec *var_dec);

    /*  
      Adds the mapping from var_name to sym_entry into the Symbol table specified by m_cur_func_iter.
      Variable is added as a function argument. If m_cur_func_iter isn't valid, returns true. 
      Otherwise returns true if a variable with var_name hasn't been declared in the most nested scope, 
      and false if a variable with var_name has been declared in the most nested scope
  */
    bool add_register_var(std::string var_name, Var_dec *var_dec);

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
    std::optional<Var_dec *> get_func_dec(std::string func_name);

    /*  Resets the scope of every symbol table to the least nested scope */
    void reset_sym_table_scopes();

    /*  Adds a new, more nested scope to the current function symbol table */
    void add_scope_to_cur_func_sym_table();

    /*  Increments the scope of the current symbol table pointed to by m_cur_func_iter */
    bool increment_scope_of_cur_fun_sym_table();

    /*  Prints every symbol table */
    void print_prog_sym_table();

    /*  Returns a vector containing the arguments to the function */
    std::vector<std::string> get_cur_func_args();

private:
    /*
      Returns an optional containing the sym_table_entry for the specified variable in the symbol table
      pointed to by m_cur_func_iter. If the specified variable isn't contained in that table, returns
      an empty optional
    */ 
    std::optional<sym_table_entry> get_sym_table_entry(const std::string& name);

    std::ofstream& m_debug_file;

    /*  Points to the function whose symbol table is currently being operated on */
    std::unordered_map<std::string, Function_symbol_table>::iterator m_cur_func_iter;

    /*  Stores all global variable declerations */
    Function_symbol_table m_global_sym_table;

    /*  Stores symbol tables for functions, mapping function name to function symbol table */
    std::unordered_map<std::string, Function_symbol_table> m_function_sym_table;
};

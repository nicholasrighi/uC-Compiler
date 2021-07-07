// Implements
#include "Dec_before_use.h"

// System includes
#include <iostream>
#include <optional>

// AST nodes
#include "../AST_classes/Array_access.h"
#include "../AST_classes/Array_dec.h"
#include "../AST_classes/Binop_dec.h"
#include "../AST_classes/Func_dec.h"
#include "../AST_classes/Func_ref.h"
#include "../AST_classes/If_dec.h"
#include "../AST_classes/Number.h"
#include "../AST_classes/Return_dec.h"
#include "../AST_classes/Stmt_dec.h"
#include "../AST_classes/Unop_dec.h"
#include "../AST_classes/Var_dec.h"
#include "../AST_classes/Var_ref.h"
#include "../AST_classes/While_dec.h"

Dec_before_use::Dec_before_use(Program_symbol_table &program_sym_table) : m_prog_sym_table(program_sym_table)
{

}

bool Dec_before_use::parse_status()
{
    return m_parse_flag;
}

/*
    Checks that array has been declared 
*/
void Dec_before_use::dispatch(Array_access &node)
{
    if (!m_prog_sym_table.get_var_dec(node.m_var->m_name))
    {
        std::cout << "Undeclared array '" << node.m_var->m_name << "'" << std::endl;
        m_parse_flag = false;
    }
}

/* 
    Adds array to local or global symbol table, depending on value of m_global_check_flag. Prints 
    error message if array has already been declared in the relevant scope
*/
void Dec_before_use::dispatch(Array_dec &node)
{
    if (m_global_var_flag)
    {
        if (!m_prog_sym_table.add_global_var(node.get_name(), &node))
        {
            std::cout << "Redeclared global array " << node.get_name() << std::endl;
            m_parse_flag = false;
        }
    }
    else
    {
        if (!m_prog_sym_table.add_local_var(node.get_name(), &node))
        {
            std::cout << "Redeclared local array " << node.get_name() << std::endl;
            m_parse_flag = false;
        }
    }
}

/*
    Dispatches the dec_before_use visitor to each child of the binop node. The operator itself
    doesn't interact with the symbol table, so no need to check that 
*/
void Dec_before_use::dispatch(Binop_dec &node)
{
    node.m_left->accept(*this);
    node.m_right->accept(*this);
}

/*
    Adds all function arguments to the symbol table. Checks function body
    to make sure all variables are declared before being used
*/
void Dec_before_use::dispatch(Func_dec &node)
{

    m_prog_sym_table.add_function(node.get_name());

    m_global_var_flag = false;

    /*  
        Function names need to be added to the global symbol table to allow them to 
        be called inside other functions
    */
    if (!m_prog_sym_table.add_global_var(node.get_name(), &node))
    {
        std::cout << "Error, function with name '" << node.get_name() << "' already declared" << std::endl;
        m_parse_flag = false;
        return;
    }

    m_prog_sym_table.add_scope_to_cur_func_sym_table();

    /*  add function args to symbol table */
    if (node.m_args != nullptr)
    {
        for (auto &func_arg : node.m_args->m_sub_expressions)
        {
            func_arg->accept(*this);
        }
    }

    node.m_func_body->accept(*this);

    m_global_var_flag = true;
}

/*
    Checks that the function has been declared. If the function hasn't been declared, 
    sets m_parse_flag to false
*/
void Dec_before_use::dispatch(Func_ref &node)
{
    if (!m_prog_sym_table.get_var_dec(node.m_var->m_name))
    {
        std::cout << "Undeclared function " << node.m_var->m_name << " used " << std::endl;
        m_parse_flag = false;
    }
}

/*
    Check condition and body for variable declerations and references. Add level to symbol table
    for new scope inside of If statement
*/
void Dec_before_use::dispatch(If_dec &node)
{
    node.m_cond->accept(*this);

    /* add a level to the symbol table for the inside of the if statement */
    m_prog_sym_table.add_scope_to_cur_func_sym_table();

    if (node.m_stmt_if_true != nullptr)
    {
        node.m_stmt_if_true->accept(*this);
    }

    if (node.m_stmt_if_false != nullptr)
    {
        node.m_stmt_if_false->accept(*this);
    }
}

/*
    Number nodes don't interact with the symbol table, so function doesn't need to do anything
*/
void Dec_before_use::dispatch(Number &node)
{
    /* no need to do anything for a constant node, since it doesn't interact with the symbol table */
}

/*
    Check that return value is only using declared variables
*/
void Dec_before_use::dispatch(Return_dec &node)
{
    node.m_return_value->accept(*this);
}

/*
    Adds all variable declerations to the symbol table, recursively checks all
    function declerations to ensure that local variables are declared before being
    used
*/
void Dec_before_use::dispatch(Stmt_dec &node)
{

    for (auto &sub_stmt : node.m_sub_expressions)
    {
        sub_stmt->accept(*this);
    }
}

/*
    Check that the expression being operated on is only using declared variables
*/
void Dec_before_use::dispatch(Unop_dec &node)
{
    node.m_exp->accept(*this);
}

/*
    Variable declerations need to be added to the symbol table. If the name is already
    present in the table we print out an error and signal that we should stop processing
*/
void Dec_before_use::dispatch(Var_dec &node)
{

    if (m_global_var_flag)
    {
        if (!m_prog_sym_table.add_global_var(node.get_name(), &node)) 
        {
            std::cout << "Redeclared global variable " << node.get_name() << std::endl;
            m_parse_flag = false;
        }
    }
    else
    {
        if (!m_prog_sym_table.add_local_var(node.get_name(), &node))
        {
            std::cout << "Redeclared local variable " << node.get_name() << std::endl;
            m_parse_flag = false;
        }
    }
}

/*
    Variable references should already be in the symbol table, so we just
    check if the variable is declared. Also need to check that the types 
    of the variable are the same (can't declare x() and then use x as in integer)
*/
void Dec_before_use::dispatch(Var_ref &node)
{
    /* check that the variable has been declared*/
    if (!m_prog_sym_table.get_var_dec(node.m_name))
    {
        std::cout << "Undeclared variable " << node.m_name << std::endl;
        m_parse_flag = false;
    }
}

/*
    Check that the condition and body of the while loop are only using declared variables. 
    Add new level to symbol table for scope inside while loop
*/
void Dec_before_use::dispatch(While_dec &node)
{
    node.m_cond->accept(*this);

    /*  Add new scope for the inside of the while loop body */
    m_prog_sym_table.add_scope_to_cur_func_sym_table();

    if (node.m_body != nullptr)
    {
        node.m_body->accept(*this);
    }
}
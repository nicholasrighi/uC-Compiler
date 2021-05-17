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

/*
    Need to check that the variable being accessed is in the symbol table
*/
void Dec_before_use::dispatch(Array_access &node)
{
    /* check that the variable has been declared*/
    if (!sym_table.get_var_dec(node.m_var->m_name))
    {
        std::cout << "Unitiliazed array '" << node.m_var->m_name << "'" << std::endl;
    }
}

/*
    Add array to symbol table and make sure that it hasn't already been declared
*/
void Dec_before_use::dispatch(Array_dec &node)
{
    if (!sym_table.add_var(node.get_name(), &node))
    {
        std::cout << "Redeclared variable " << node.get_name() << std::endl;
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

    /* add the function itself to the symbol table*/
    if (!sym_table.add_var(node.get_name(), &node))
    {
        std::cout << "Error, variable with name '" << node.get_name() << "' already declared" << std::endl;
    }

    /* now add chain new symbol table for local function scope */
    sym_table.add_level();

    if (node.m_args != nullptr)
    {
        /* iterate through all function arguments and dispatch visitor to them */
        for (auto &func_arg : node.m_args->m_sub_expressions)
        {
            func_arg->accept(*this);
        }
    }

    /* check func body */
    node.m_func_body->accept(*this);

    /* now remove most nested symbol table to remove references to local functions */
    sym_table.remove_level();
}

/*
    Check that the function has already been declared 
*/
void Dec_before_use::dispatch(Func_ref &node)
{
    /* check that the function is already declared */
    
    if (!sym_table.get_var_dec(node.m_var->m_name))
    {
        std::cout << "Undeclared function " << node.m_var->m_name << " used " << std::endl;
    }
}

/*
    Check condition and body for variable declerations and references
*/
void Dec_before_use::dispatch(If_dec &node)
{
    node.m_cond->accept(*this);
    node.m_stmt_if_true->accept(*this);

    /* not always an 'else' part, need to check if there is one before calling it */
    if (node.m_stmt_if_false == nullptr)
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
    if (!sym_table.add_var(node.get_name(), &node))
    {
        std::cout << "Redeclared variable " << node.get_name() << std::endl;
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
    if (!sym_table.get_var_dec(node.m_name))
    {
        std::cout << "Unitiliazed variable " << node.m_name << std::endl;
    }
}

/*
    Check that the condition and body of the while loop are only using declared variables
*/
void Dec_before_use::dispatch(While_dec &node)
{
    node.m_cond->accept(*this);
    node.m_body->accept(*this);
}
// Implements
#include "Dec_before_use.h"

// System includes
#include <iostream>

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
    Need to check that the variable being accessed is in the symbol table, and that the 
    variable is an array
*/
void Dec_before_use::dispatch(Array_access &node)
{
}

/*

*/
void Dec_before_use::dispatch(Array_dec &node) {}

/*

*/
void Dec_before_use::dispatch(Binop_dec &node) {}

/*
    Adds all function arguments to the symbol table. Checks function body
    to make sure all variables are declared before being used
*/
void Dec_before_use::dispatch(Func_dec &node)
{
    if (node.m_args != nullptr)
    {
        /* iterate through all function arguments and dispatch visitor to them */
        for (auto &func_arg : node.m_args->m_sub_expressions)
        {
            func_arg->accept(*this);
        }
    }
}

/*

*/
void Dec_before_use::dispatch(Func_ref &node) {}

/*

*/
void Dec_before_use::dispatch(If_dec &node) {}

/*

*/
void Dec_before_use::dispatch(Number &node) {}

/*

*/
void Dec_before_use::dispatch(Return_dec &node) {}
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

*/
void Dec_before_use::dispatch(Unop_dec &node) {}

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
    else
    {
        std::cout << "Added var '" << node.get_name() << "' with type " << node.get_type_string() << std::endl;
    }
}

/*

*/
void Dec_before_use::dispatch(Var_ref &node) {}

/*

*/
void Dec_before_use::dispatch(While_dec &node) {}
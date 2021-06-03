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

Dec_before_use::Dec_before_use(std::list<Symbol_table> &sym_table) : m_parse_flag(true), m_global_var_flag(true), m_sym_table_list(sym_table), m_bsp_offset(0)
{
    /* give the type checker access to the symbol table that the dec before use generates*/
    //m_type_check_visitor.assign_sym_table(&sym_table);
    m_global_check_flag = true;
}

bool Dec_before_use::global_parse_status()
{
    return m_global_check_flag;
}

void Dec_before_use::gen_3_code(Base_node *root)
{
    //root->accept(m_three_code_gen);
}

void Dec_before_use::print()
{
    //m_three_code_gen.print_IR_code();
}

bool Dec_before_use::parse_status()
{
    return m_parse_flag; //&& m_type_check_visitor.parse_status() && m_return_checker.parse_status();
}

/*
    Need to check that the variable being accessed is in the symbol table
*/
void Dec_before_use::dispatch(Array_access &node)
{
    /* check that the variable has been declared*/
    if (!m_sym_table_list.back().get_var_dec(node.m_var->m_name))
    {
        std::cout << "Undeclared array '" << node.m_var->m_name << "'" << std::endl;
        m_parse_flag = false;
    }
}

/*
    Add array to symbol table and make sure that it hasn't already been declared
*/
void Dec_before_use::dispatch(Array_dec &node)
{
    Var_storage storage_type = m_global_check_flag ? Var_storage::GLOBAL : Var_storage::LOCAL;

    if (!m_sym_table_list.back().add_var(node.get_name(), sym_table_entry(&node, storage_type, m_bsp_offset)))
    {
        std::cout << "Redeclared variable " << node.get_name() << std::endl;
        m_parse_flag = false;
    }
    m_bsp_offset += node.m_array_size;
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

    /*  Need a new symbol table for this function */
    m_sym_table_list.push_back(Symbol_table {});

    /* set to false to prevent locals from being declared global. Reset to true on function exit */
    m_global_var_flag = false;

    /* add the function itself to the symbol table. Storage type and offset don't matter for functions */
    if (!m_sym_table_list.back().add_var(node.get_name(), sym_table_entry(&node, Var_storage::GLOBAL, 0)))
    {
        std::cout << "Error, variable with name '" << node.get_name() << "' already declared" << std::endl;
        m_parse_flag = false;
    }

    /* now add chain new symbol table for local function scope */
    m_sym_table_list.back().add_level();

    /*  add function args to symbol table */
    if (node.m_args != nullptr)
    {
        for (auto &func_arg : node.m_args->m_sub_expressions)
        {
            func_arg->accept(*this);
        }
    }

    node.m_func_body->accept(*this);

    /* now that we've generated the symbol table we can run the type checker */
    //node.accept(m_type_check_visitor);

    /* 
        now that the type checker has verified that all types are correct, we can 
        verify that there is a return statement through all paths of control flow
        for non-void functions
    */
    /*
    if (node.m_var_type != Ret_type::VOID)
    {
        node.accept(m_return_checker);
    }
    */

    /* marks if any of the visitor classes failed, ensures failures persist through entire visitor check */
    if (!parse_status())
    {
        m_global_check_flag = false;
    }

    m_global_var_flag = true;
}

/*
    Checks that the function has been declared and that the function arguments being passed
    to the function are compatible with the function's decleration
*/
void Dec_before_use::dispatch(Func_ref &node)
{

    /* get func dec from symbol table */
    std::optional<Var_dec *> func_dec = m_sym_table_list.back().get_var_dec(node.m_var->m_name);

    /* check that the function is already declared */
    if (!func_dec)
    {
        std::cout << "Undeclared function " << node.m_var->m_name << " used " << std::endl;
        m_parse_flag = false;

        /* prevents from evaluating function that hasn't been declared, and thus can't type check args */
        return;
    }
}

/*
    Check condition and body for variable declerations and references. Add level to symbol table
    for new scope inside of If statement
*/
void Dec_before_use::dispatch(If_dec &node)
{
    /* condition is part of current scope, check for variables in current scope */
    node.m_cond->accept(*this);

    /* add a level to the symbol table for the inside of the if statement */
    m_sym_table_list.back().add_level();

    /* check that the body of the if statement isn't null and examine the body */
    if (node.m_stmt_if_true != nullptr)
    {
        node.m_stmt_if_true->accept(*this);
    }

    /* not always an 'else' part, need to check if there is one before calling it */
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

    Var_storage storage_type = m_global_var_flag ? Var_storage::GLOBAL : Var_storage::LOCAL;

    /*  
        Add variable to the symbol table for global variables if var is declared as global. Otherwise add to the 
        function specific symbol table and add 8 to the offset 
    */
    if (m_global_var_flag)
    {
        if (!m_sym_table_list.front().add_var(node.get_name(), sym_table_entry(&node, storage_type, m_bsp_offset)))
        {
            std::cout << "Redeclared global variable " << node.get_name() << std::endl;
            m_parse_flag = false;
        }
    }
    else
    {
        if (m_sym_table_list.back().add_var(node.get_name(), sym_table_entry(&node, storage_type, m_bsp_offset)))
        {
            m_bsp_offset += 8;
        }
        else
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
    if (!m_sym_table_list.back().get_var_dec(node.m_name))
    {
        std::cout << "Undeclared variable " << node.m_name << std::endl;
        m_parse_flag = false;
    }
}

/*
    Check that the condition and body of the while loop are only using declared variables. Add new level to symbol
    table for scope inside while loop
*/
void Dec_before_use::dispatch(While_dec &node)
{
    /* the condition uses the current scope, so don't add a level to the symbol table*/
    node.m_cond->accept(*this);

    /* the body of the while loop is a new scope, so we add a level to the symbol table */
    m_sym_table_list.back().add_level();

    /* check that loop body exists, and then type check the body */
    if (node.m_body != nullptr)
    {
        node.m_body->accept(*this);
    }
}
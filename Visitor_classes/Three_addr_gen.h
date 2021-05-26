#pragma once

// Inherits from
#include "Abstract_visitor.h"

// System includes
#include <list>
#include <string>
#include <optional>

// Local includes
#include "../Supporting_classes/Symbol_table.h"
#include "Three_addr_var.h"

enum class Three_addr_OP
{
  MOVE,
  LOAD,
  STORE,
  LOG_AND,
  BIT_AND,
  LOG_OR,
  BIT_OR,
  ADD,
  SUB,
  MULT,
  DIVIDE,
  UMINUS,
  LESS_THAN,
  GREATER_THAN,
  LOG_INVERT,
  RET,
  LABEL // LABEL indicates that the instruction is simply the destination of some jump
};

/*  Returns the string representation of a given three op code */
std::string three_op_to_string(Three_addr_OP op);

/*  
  Entry for three address code in intermediate representation. 
  Order is new temporary variable, operator, temporary variable 1, temporary variable 2 
*/
using three_addr_code_entry = std::tuple<Three_addr_var, Three_addr_OP, Three_addr_var, Three_addr_var>;

class Three_addr_gen : public Abstract_visitor
{
public:
  Three_addr_gen(Symbol_table *sym_table);

  void dispatch(Array_access &node) override;
  void dispatch(Array_dec &node) override;
  void dispatch(Binop_dec &node) override;
  void dispatch(Func_dec &node) override;
  void dispatch(Func_ref &node) override;
  void dispatch(If_dec &node) override;
  void dispatch(Number &node) override;
  void dispatch(Return_dec &node) override;
  void dispatch(Stmt_dec &node) override;
  void dispatch(Unop_dec &node) override;
  void dispatch(Var_dec &node) override;
  void dispatch(Var_ref &node) override;
  void dispatch(While_dec &node) override;

  void print_IR_code();

private:
  /* returns the next avaliable temporary variable */
  std::string gen_temp()
  {
    int stored_index = m_temp_index;
    m_temp_index++;
    return "t" + std::to_string(stored_index);
  }

  /* Returns the next avaliable label  */
  std::string gen_label()
  {
    int stored_index = m_label_index;
    m_label_index++;
    return "L" + std::to_string(stored_index);
  }

  /* The three address entry generated by the most recently visited child node */
  Three_addr_var m_last_entry;

  /*  keeps track of the next value for a temporary variable */
  int m_temp_index;

  /*  keeps track of the next value for a label */
  int m_label_index;

  /*  symbol table for variable definitions */
  Symbol_table *m_sym_table;

  /*  stores the intermediate representation of the program */
  std::list<three_addr_code_entry> m_intermediate_rep;
};

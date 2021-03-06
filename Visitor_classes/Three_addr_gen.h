#pragma once

// Inherits from
#include "Abstract_visitor.h"

// System includes
#include <vector>
#include <string>
#include <optional>
#include <fstream>
#include <set>

// Local includes
#include "../Supporting_classes/Program_symbol_table.h"
#include "Three_addr_var.h"

enum class Three_addr_OP
{
  RAW_STR, 
  BACK_PATCH_DEC,
  BACK_PATCH_INC,
  CALL,
  MOVE,
  CMP,
  LOAD,
  LOAD_ARG,
  SAVE_REGS,
  RESTORE_REGS,
  RETURN_VAL,
  ASSIGN,
  EQUALITY,
  NOT_EQUALITY,
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
  LESS_THAN_EQUAL,
  GREATER_THAN,
  GREATER_THAN_EQUAL,
  LOG_INVERT,
  RET,
  UNCOND_J,
  EQUAL_J,
  NEQUAL_J,
  LABEL 
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
  Three_addr_gen(std::ofstream& debug_log, Program_symbol_table &sym_table,
                 std::vector<std::vector<three_addr_code_entry>> &IR_code);

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

  /* Writes the Three_addr_code output to the debug log */
  void print_IR_code();


private:

  /*  Removes any instructions from m_intermediate_rep following a return statement that won't be executed */
  void remove_op_trailing_return();

  /*  Removes labels that aren't the targets of any jmp instructions from m_intermediate_rep */ 
  void remove_unused_labels();

  /*  
      Replaces adjacent labels with a single label. Replaces all references to deleted labels with references
      to the new labels
  */
  void merge_adjacent_labels();

  /*
      Replaces all instances of Three_addr_OP::BACKPATCH_INC and Three_addr_OP::BACKPATCH_DEC with an 
      increment or decrement of the stack pointer by the offset returned by the symbol table
  */ 
  void backpatch_offsets();

  /* returns the next avaliable temporary variable */
  std::string gen_temp();

  /* Returns the next avaliable label  */
  std::string gen_label();

  /* The three address entry generated by the most recently visited child node */
  Three_addr_var m_last_entry;

  /*  keeps track of the next value for a temporary variable */
  int m_temp_index;

  /*  keeps track of the next value for a label */
  int m_label_index;

  /*  symbol table for variable definitions */
  Program_symbol_table &m_sym_table;

  /*  stores the intermediate representation of the program */
  std::vector<std::vector<three_addr_code_entry>> &m_intermediate_rep;

  /*  contains all function names in the input file */ 
  std::set<std::string> m_func_names;

  /*  file that contains output of print_IR_code() */
  std::ofstream& m_debug_log;

  /*  indicates if the immediate left or right child of a Binop node is an array */
  bool m_child_is_array_dec = false;

  /*  Always load an array value except when an array is on the left hand side of an "=" operator */
  bool m_load_array_value = true;

  /*  Used to determine how to evaulate if an expression has a true of false truth value */
  bool m_child_is_conditional = false;
};

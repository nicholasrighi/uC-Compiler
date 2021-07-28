#pragma once

/*  System includes */
#include <fstream>
#include <iostream>
#include <stack>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

/*  Local includes */
#include "../Visitor_classes/Three_addr_gen.h"
#include "CFG_node.h"
#include "Program_symbol_table.h"

/*  General purpose x86 registers for allocation (doesn't include rax, rsp, or
 * rbp). Currently only allocate for r8-r15 */
enum class x86_Register
{
  r8,
  r9,
  r10,
  r11,
  r12,
  r13,
  r14,
  RCX,
  RBX,
  RSI,
  RDI,
  RDX
};

/*  Returns the string representation of an x86 register */
std::string x86_Register_to_string(x86_Register);

/*
    Stores a CFG_node and a flag indicating if that node has already had
    assembly generated for it
*/
using CFG_entry = std::pair<CFG_node, bool>;

/*
  Stores data needed for allocating variabels to registers. Format is:
    Three_addr_var =      The variable or constant being stored in the register
    x86_register =        physical register that variable name is mapped to
    std::optional<int> =  next nearest instruction that uses that variable (if
                              variable is used again)
*/
using register_entry = std::tuple<Three_addr_var, x86_Register, std::optional<int>>;

/*
  This class is responsible for taking a container of 3 addr codes and
  generating an assembly file that implements those 3 addr codes
*/
class Reg_allocator
{
public:
  /*  Creates a reg allocator that generates an assembly file with the specified
   * name */
  Reg_allocator(std::string asm_file_name,
                std::ofstream &debug_log,
                Program_symbol_table &sym_table,
                std::vector<std::vector<three_addr_code_entry>> &three_addr_code,
                std::vector<std::string>& global_vars);

  ~Reg_allocator();

  /*
      Takes an input list of 3 addr codes, converts the 3 addr codes to x86-64
     assembly, and allocates physical resgisters to the resulting assembly
  */
  void generate_asm_file();

private:
  void write_global_vars_to_asm();

  /*  adds regsiters currently in use to m_caller_saved_regs */
  void mark_in_use_regs();

  /*  Allocates function arguments to registers */
  void setup_initial_regs(const CFG_node &node,
                          std::vector<three_addr_code_entry> &IR_code);

  /*  Loads function arguments into the appropriate registers for the function call */
  void move_func_args_to_regs();

  /*  Print the CFG nodes to the file */
  void print_CFG();

  /*  Prints the contents of the registers to the debug log */
  void print_register_contents();

  /*  Helper function for printing the next use of a variable to the debug log */
  void print_next_var_use(Three_addr_var &var,
                          std::optional<int> next_use);

  /*  Helper function for retrieving the Three_address var from a register entry */
  Three_addr_var &reg_entry_var(register_entry &);

  /*  Helper function for retrieving the x86_Register from a register entry */
  x86_Register &reg_entry_reg(register_entry &);

  /*  Retrieves the distance to the next use of that variable from a register entry */
  std::optional<int> &reg_entry_dist(register_entry &);

  /*
     Determines which variables should be added to the var_kill set and which
     variables should be added to the UEVar set for each node in the CFG corresponding to the 
     three_addr_code entry
  */
  void generate_varkill_uevar(std::vector<three_addr_code_entry> &);

  /*  Empties the CFG data members */
  void reset_cfg_data_members();

  /*  Empties the register data */
  void reset_registers();

  /*  Generates a control flow graph for m_three_addr_code from the passed vector */
  void generate_CFG(std::vector<three_addr_code_entry> &);

  /*  Generates a live out set for each basic block in the CFG based on the specified three_addr_code_entry*/
  void generate_live_out_overall(std::vector<three_addr_code_entry> &);

  /*  
      Generates the liveout set for the specified CFG_node based on all sucessors of the specified node 
      Returns true if the live out set of the specified node has changed, false otherwise
  */
  bool generate_live_out_from_node(CFG_node &node);

  /*  Generates assembly based on the CFG node and the LiveOut set of each basic
   * block */
  void generate_assembly_from_CFG_node(CFG_node &node,
                                       std::vector<three_addr_code_entry> &IR_code);

  /*
      Saves all variables in node's live out set that reside in registers to memory
      Then frees those registers
  */
  void save_live_out_vars(CFG_node &node);

  /*  Generates a single line of assembly using up to three registers and one op code */
  void generate_asm_line(std::optional<x86_Register>,
                         Three_addr_OP, std::optional<x86_Register>,
                         std::optional<x86_Register>,
                         std::string jmp_target = "");

  /*
    Allocates a register for the specified variable. If the variable is already
    stored in a register it returns that register. Otherwise it spills the
    variable that will be used furthest into the future into memory and returns
    that now free register. If var_to_be_allocated is a label or a raw string, then
    ensure allocates nothing and returns an empty optional
  */
  std::optional<x86_Register> ensure(const Three_addr_var &var_to_be_alloced);

  /*
     Finds and returns the x86_Register that will be used the furthest in the
     future. If that register contains a variable that will be used in the
     future, stores that variable to memory. Adds the passed Three_addr_var into
     m_allocated_reg_data
  */
  x86_Register allocate_reg(const Three_addr_var &var_to_be_allocated);

  /*
      Frees the specified register by setting it to free in m_register_free_status and 
      putting the register onto m_free_reg_stack
  */
  void free(x86_Register reg_to_free,
            CFG_node &node);

  /*
    Stores the value allocated in var_to_free to the offset specified in the symbol table. If the variable
    contained in var_to_free isn't in the symbol table, store_reg() adds the variable to the symbol table and 
    stores the variable at the new offset
  */
  void store_reg(x86_Register reg_to_free);

  /*  Emits assembly to load the value stored in var_to_be_allocated into free_reg */
  void load_reg(const Three_addr_var &var_to_be_allocated,
                x86_Register free_reg);

  /*
      If the specified variable is contained in a register returns that register. Otherwise returns
      an empty optional
  */
  std::optional<x86_Register> find_allocated_var(const Three_addr_var &var_to_be_allocated);

  /*
    Returns the next instruction at which var_name is used. If var_name isn't
    used after start_index, returns an empty optional
  */
  std::optional<int> dist_to_next_var_occurance(const Three_addr_var &search_var,
                                                int start_index,
                                                const CFG_node &node,
                                                const std::vector<three_addr_code_entry> &IR_code);

  /*
    Returns the specified variable's offset in the symbol table. If the variable
    isn't in the symbol table then it adds the variable and returns the offset
  */
  int get_var_offset_cond_add(std::string var_name);

  /*  output file for assembly code */
  std::ofstream m_asm_file;

  /*  holds logging information to aid with debugging */
  std::ofstream &m_debug_log;

  /*  
      Holds user and temporary variables, as well as their offsets from the base pointer
      on the stack
  */
  Program_symbol_table &m_prog_sym_table;

  /*  Vector of three address codes that will be translated into assembly */
  std::vector<std::vector<three_addr_code_entry>> &m_three_addr_code;

  /*  Vector of basic blocks. Each block will have assembly generated for it */
  std::vector<CFG_entry> m_cfg_graph;

  /*  
      Maps the start index of a cfg block to that node's index inside m_cfg_graph 
        1st int:  start index
        2nd int:  index of cfg node in m_cfg_graph
  */
  std::unordered_map<int, int> m_start_index_to_graph_index;

  /*
      m_allocated_reg_data[i] says what variable x86_Register(i) is holding, if
      that variable is still in use, and the next closest instruction that
      variable will be used in
  */
  std::vector<register_entry> m_allocated_reg_data;

  /*
      m_register_free_status[i] is true if the register in m_allocated_reg_data[i] is free.
      Otherwise is false
  */
  std::vector<bool> m_register_free_status;

  /*  Holds free x86 registers */
  std::stack<x86_Register> m_free_reg_stack;

  /*  Holds saved registers that are part of the calling convention before a function call */
  std::vector<x86_Register> m_caller_saved_regs;

  /*  Holds general registers that are saved before a function call */
  std::vector<x86_Register> m_general_saved_regs;

  /*  Registers used by the x86_64 calling convention for passing the first 6 arguments to a function */
  const std::vector<x86_Register> m_calling_convention_regs = {x86_Register::RDI,
                                                               x86_Register::RSI,
                                                               x86_Register::RDX,
                                                               x86_Register::RCX,
                                                               x86_Register::r8,
                                                               x86_Register::r9};

  /*  Registers not used in x86_calling convention */
  const std::vector<x86_Register> m_regs_not_containing_args = {x86_Register::RBX,
                                                                x86_Register::r10,
                                                                x86_Register::r11,
                                                                x86_Register::r12,
                                                                x86_Register::r13,
                                                                x86_Register::r14};

  int m_calling_vec_index = 0;

  const int m_last_reg_index = static_cast<int>(x86_Register::RDX);

  std::vector<std::string>& m_global_vars;
};
#pragma once

/*  System includes */
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <stack>

/*  Local includes */
#include "../Visitor_classes/Three_addr_gen.h"
#include "Program_symbol_table.h"
#include "CFG_node.h"

/*  General purpose x86 registers for allocation (doesn't include rax, rsp, or rbp). Currently only allocate for r8-r15 */
enum class x86_Register
{
  r8,
  r9,
  r10,
  r11,
  r12,
  r13,
  r14,
  r15,
  RCX,
  RDX,
  RBX,
  RSI,
  RDI
};

/*  Returns the string representation of an x86 register */
std::string x86_Register_to_string(x86_Register);

/*  
    Stores a CFG_node and a flag indicating if that node has already had assembly 
    generated for it 
*/
using CFG_entry = std::pair<CFG_node, bool>;

/*
  Stores data needed for allocating variabels to registers. Format is:
    Three_addr_var =      The variable or constant being stored in the register 
    x86_register =        physical register that variable name is mapped to
    std::optional<int> =  next nearest instruction that uses that variable (if variable is used again)
*/
using register_entry = std::tuple<Three_addr_var, x86_Register, std::optional<int>>;

/*
  This class is responsible for taking a container of 3 addr codes and generating an assembly file 
  that implements those 3 addr codes
*/
class Reg_allocator
{

public:
  /*  Creates a reg allocator that generates an assembly file with the specified name */
  Reg_allocator(std::string file_name, Program_symbol_table &sym_table, std::vector<three_addr_code_entry> &three_addr_code);

  ~Reg_allocator();

  /*  
      Takes an input list of 3 addr codes, converts the 3 addr codes to x86-64 assembly, and allocates
      physical resgisters to the resulting assembly 
  */
  void generate_asm_file();

private:
  /*  
      Determines which variables should be added to the var_kill set and which variables should 
      be added to the UEVar set for each node in the CFG
  */
  void generate_varkill_uevar(CFG_node &node);

  /*  Generates a control flow graph for m_three_addr_code */
  void generate_CFG();

  /*  Generates a live out set for each basic block in the CFG */
  void create_live_out();

  /*  Generates assembly based on the CFG node and the LiveOut set of each basic block */
  void generate_assembly_from_CFG_node(CFG_node& node);

  /*  Generates a single line of assembly using up to three registers and one op code */
  void gen_asm_line(x86_Register, Three_addr_OP, x86_Register, x86_Register);

  /*  
    Allocates a register for the specified variable. If the variable is already stored in a 
    register it returns that register. Otherwise it spills the variable that will be used
    furthest into the future into memory and returns that now free register
  */
  x86_Register ensure(Three_addr_var var_to_be_alloced);

  /*  
      Finds and returns the x86_Register that will be used the furthest in the future.
      If that register contains a variable that will be used in the future, stores
      that variable to memory. Adds the passed Three_addr_var into m_allocated_reg_data
  */
  x86_Register allocate(Three_addr_var var_to_be_allocated);

  /*  
      Frees the physical register specified in register entry. If the variable being spilled is used
      in the future, then free writes that variable to the stack (either allocating space for the 
      variable if necessary, or adding the variable to the symbol table if it's not already there)
      Then free adds the newly freed physical register to m_free_physical_registers
  */
  void free(register_entry &reg_entry);

  /*  
    Returns the next instruction at which var_name is used. If var_name isn't used after 
    start_index, returns an empty optional
  */
  std::optional<int> dist_to_next_var_occurance(Three_addr_var search_var, int start_index, CFG_node& node);

  /*
    Returns the specified variable's offset in the symbol table. If the variable isn't
    in the symbol table then it adds the variable and returns the offset
  */
  int get_var_offset_cond_add(std::string var_name);

  /*  output file for assembly code */
  std::ofstream m_asm_file;

  Program_symbol_table &m_prog_sym_table;

  std::vector<three_addr_code_entry> &m_three_addr_code;

  std::vector<CFG_entry> m_cfg_graph;

  /*  
      m_allocated_reg_data[i] says what variable x86_Register(i) is holding, if that 
      variable is still in use, and the next closest instruction that variable will be used
      in
  */
  std::vector<register_entry> m_allocated_reg_data;

  /*  
      m_register_free_status[i] is true if the register in m_allocated_reg_data[i] 
      is free (contains no data that will be used later), otherwise is false
  */
  std::vector<bool> m_register_free_status;

  /*  Holds free x86 registers */
  std::stack<x86_Register> m_free_reg_stack;
};
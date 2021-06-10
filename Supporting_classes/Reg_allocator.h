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

/*  
    Stores a CFG_node and a flag indicating if that node has already had assembly 
    generated for it 
*/
using CFG_entry = std::pair<CFG_node, bool>;

/*
  Stores data needed for allocating variabels to registers. Format is 
  allocated_var_name, physical register var_name is mapped to , index at which var is next used, 
  flag indicating if entry is still in use
*/
using register_entry = std::tuple<std::string, std::string, std::optional<int>, bool>;

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
  void gen_asm(std::vector<three_addr_code_entry> &three_addr_codes);

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

  /*  Generates assembly based on the CFG and the LiveOut set of each basic block */
  void generate_assembly(CFG_node);

  /*  
    Allocates a register for the specified variable. If the variable is already stored in a 
    register it returns that register. Otherwise it spills the variable that will be used
    furthest into the future into memory and returns that now free register
  */
  std::string ensure(std::string var_name);

  /*  
      Allocates a register for the specified variable. If all registers are full then this
      spills the register that will be used the farthest into the future into memory and returns
      the now free register.
  */
  std::string allocate(std::string var_name);

  /*  
      Returns the name of the temporary variable in m_allocated_reg_data that 
      is used the farthest in the future 
  */
  std::string dist();

  /*  output file for assembly code */
  std::ofstream m_asm_file;

  Program_symbol_table &m_prog_sym_table;

  std::vector<three_addr_code_entry> &m_three_addr_code;

  std::vector<CFG_entry> m_cfg_graph;

  /*  Stores data on which temporary variables are being allocated to which registers */
  std::vector<register_entry> m_allocated_reg_data; 

  std::stack<std::string> m_free_physical_registers;
};
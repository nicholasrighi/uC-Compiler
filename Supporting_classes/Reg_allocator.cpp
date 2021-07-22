#include "Reg_allocator.h"

#include <algorithm>

std::string x86_Register_to_string(x86_Register reg)
{
  switch (reg)
  {
  case x86_Register::r8:
    return "%r8";
    break;
  case x86_Register::r9:
    return "%r9";
    break;
  case x86_Register::r10:
    return "%r10";
    break;
  case x86_Register::r11:
    return "%r11";
    break;
  case x86_Register::r12:
    return "%r12";
    break;
  case x86_Register::r13:
    return "%r13";
    break;
  case x86_Register::r14:
    return "%r14";
    break;
  case x86_Register::RCX:
    return "%rcx";
    break;
  case x86_Register::RBX:
    return "%rbx";
    break;
  case x86_Register::RSI:
    return "%rsi";
    break;
  case x86_Register::RDI:
    return "%rdi";
    break;
  case x86_Register::RDX:
    return "%rdx";
    break;
  default:
    return "Error, invalid register name passed to x86_Register_to_string function";
    break;
  }
}

Reg_allocator::Reg_allocator(std::string asm_file_name, std::ofstream &debug_log,
                             Program_symbol_table &sym_table,
                             std::vector<std::vector<three_addr_code_entry>> &three_addr_code)
    : m_debug_log(debug_log), m_prog_sym_table(sym_table), m_three_addr_code(three_addr_code)
{
  m_asm_file.open(asm_file_name, std::ofstream::trunc);

  m_asm_file << ".globl main" << std::endl;

  m_register_free_status = std::vector<bool>(m_last_reg_index + 1, true);

  /*  
      Needs to <= RDX since we need RDX to be added to the register list as well as all the other registers 
      Don't setup the free register stack, since free register stack is setup when registers are allocated
      for function arguments
  */
  for (int i = 0; i <= m_last_reg_index; i++)
  {
    x86_Register cur_reg = static_cast<x86_Register>(i);
    m_allocated_reg_data.push_back(std::make_tuple(Three_addr_var(), cur_reg, std::nullopt));
  }
}

Reg_allocator::~Reg_allocator()
{
  m_asm_file.close();
}

void Reg_allocator::generate_asm_file()
{
  m_debug_log << "\nStart of debug log for assembly\n"
              << std::endl;
  for (std::vector<three_addr_code_entry> &cur_IR_code : m_three_addr_code)
  {
    /*  The first entry in each function is a label containing the function name */
    m_prog_sym_table.set_search_func(std::get<2>(cur_IR_code.front()).to_string());
    reset_cfg_data_members();
    generate_CFG(cur_IR_code);
    generate_live_out_overall(cur_IR_code);
    print_CFG();
    for (int i = 0; i < m_cfg_graph.size(); i++)
    {
      /*  
          Registers need to be reset after every basic block to ensure that the 
          correct values are loaded from memory, regardless of where the 
          control flow was before the current basic block
      */
      auto &CFG_node = m_cfg_graph.at(i);
      if (i == 0)
      {
        m_calling_vec_index = 0;
        setup_initial_regs(CFG_node.first, cur_IR_code);
      }
      else
      {
        reset_registers();
      }

      if (!CFG_node.second)
      {
        generate_assembly_from_CFG_node(CFG_node.first, cur_IR_code);
      }
    }
  }
  m_asm_file << std::endl;
}

void Reg_allocator::print_register_contents()
{
  m_debug_log << std::endl;
  for (register_entry &cur_reg_entry : m_allocated_reg_data)
  {

    int reg_to_index = static_cast<int>(reg_entry_reg(cur_reg_entry));
    std::string reg_as_str = x86_Register_to_string(reg_entry_reg(cur_reg_entry));
    std::optional<int> next_var_use = reg_entry_dist(cur_reg_entry);

    if (m_register_free_status.at(reg_to_index))
    {
      m_debug_log << reg_as_str << ": unallocated" << std::endl;
    }
    else
    {
      m_debug_log << reg_as_str << ": " << reg_entry_var(cur_reg_entry).to_string()
                  << " next use at";
      if (next_var_use.has_value())
      {
        m_debug_log << " index " + std::to_string(next_var_use.value());
      }
      else
      {
        m_debug_log << " not used again";
      }
      m_debug_log << std::endl;
    }
  }

  m_debug_log << std::endl;
}

void Reg_allocator::mark_in_use_regs()
{
  /*  Mark in use registers that are part of the calling convention but aren't used for this function call */
  for (int i = m_calling_vec_index; i < m_calling_convention_regs.size(); i++)
  {
    int cur_reg_as_index = static_cast<int>(m_calling_convention_regs.at(i));
    if (!m_register_free_status.at(cur_reg_as_index))
    {
      m_general_saved_regs.push_back(static_cast<x86_Register>(cur_reg_as_index));
    }
  }

  /*  Mark in use registers that aren't part of the calling convention */
  for (x86_Register cur_reg : m_regs_not_containing_args)
  {
    if (!m_register_free_status.at(static_cast<int>(cur_reg)))
    {
      m_general_saved_regs.push_back(cur_reg);
    }
  }
}

void Reg_allocator::setup_initial_regs(const CFG_node &node,
                                       std::vector<three_addr_code_entry> &IR_code)
{
  for (int cur_arg = 0; cur_arg < m_prog_sym_table.get_cur_func_args().size(); cur_arg++)
  {
    std::string var_name = m_prog_sym_table.get_cur_func_args().at(cur_arg);
    Three_addr_var func_arg;

    if (m_prog_sym_table.get_var_dec(var_name).value()->m_obj_type == Object_type::ARRAY)
    {
      func_arg = Three_addr_var(var_name, Three_addr_var_type::ARRAY);
    }
    else
    {
      func_arg = Three_addr_var(var_name);
    }

    m_register_free_status.at(static_cast<int>(m_calling_convention_regs.at(cur_arg))) = false;

    register_entry &cur_entry =
        m_allocated_reg_data.at(static_cast<int>(m_calling_convention_regs.at(cur_arg)));

    reg_entry_var(cur_entry) = func_arg;
    reg_entry_dist(cur_entry) = dist_to_next_var_occurance(func_arg, 0, node, IR_code);
    reg_entry_reg(cur_entry) = m_calling_convention_regs.at(cur_arg);
  }

  int first_unused_index = m_prog_sym_table.get_cur_func_args().size();

  for (int i = first_unused_index; i < m_calling_convention_regs.size(); i++)
  {
    m_free_reg_stack.push(m_calling_convention_regs.at(i));
  }
  for (x86_Register reg : m_regs_not_containing_args)
  {
    m_free_reg_stack.push(reg);
  }
}

void Reg_allocator::print_CFG()
{
  for (CFG_entry &cfg_node : m_cfg_graph)
  {
    m_debug_log << "Start index: " << std::to_string(cfg_node.first.m_start_index) << std::endl;
    m_debug_log << "End index: " << std::to_string(cfg_node.first.m_end_index) << std::endl;
    m_debug_log << "Varkill set:" << std::endl;

    for (const std::string &str : cfg_node.first.m_var_kill)
    {
      m_debug_log << "\t" + str << std::endl;
    }

    m_debug_log << "UEVar set:" << std::endl;
    for (const std::string &str : cfg_node.first.m_uevar)
    {
      m_debug_log << "\t" + str << std::endl;
    }

    m_debug_log << "Liveout set:" << std::endl;
    for (const std::string &str : cfg_node.first.m_live_out)
    {
      m_debug_log << "\t" + str << std::endl;
    }

    m_debug_log << "\n"
                << std::endl;
  }
}

/*  Assumes all operations are of the form "x <- y op z", where y and z are optional */
void Reg_allocator::generate_varkill_uevar(std::vector<three_addr_code_entry> &IR_code)
{

  auto is_local_array = [this](Three_addr_var &var)
  {
    std::optional<Var_dec *> local_var_dec = this->m_prog_sym_table.get_var_dec(var.to_string());

    if (local_var_dec.value() == nullptr)
    {
      return false;
    }

    std::vector<std::string> func_args = this->m_prog_sym_table.get_cur_func_args();

    int is_local_var = find(func_args.begin(), func_args.end(), var.to_string()) == func_args.end();

    return local_var_dec.value()->m_obj_type == Object_type::ARRAY &&
           is_local_var;
  };

  for (CFG_entry &cur_entry : m_cfg_graph)
  {
    CFG_node &node = cur_entry.first;

    /*  The first node in the CFG graph needs to add the function args to varkill */
    if (cur_entry.first == m_cfg_graph.front().first)
    {
      for (std::string &cur_arg : m_prog_sym_table.get_cur_func_args())
      {
        node.m_var_kill.insert(cur_arg);
      }
    }

    for (int i = node.m_start_index; i <= node.m_end_index; i++)
    {
      Three_addr_var x = std::get<0>(IR_code[i]);
      Three_addr_var y = std::get<2>(IR_code[i]);
      Three_addr_var z = std::get<3>(IR_code[i]);

      /*  If a variable is defined in a basic block then it's added to the varkill set */
      if (x.is_var() && !is_local_array(x))
      {
        node.m_var_kill.insert(x.to_string());
      }

      /*  
          If variables y and z are used but aren't in varkill, they must be defined in an earlier
          basic block. This means they need to be added to ueavar 
      */
      if (y.is_var() && !is_local_array(y) && (node.m_var_kill.count(y.to_string()) == 0))
      {
        node.m_uevar.insert(y.to_string());
      }

      if (z.is_var() && !is_local_array(z) && (node.m_var_kill.count(z.to_string()) == 0))
      {
        node.m_uevar.insert(z.to_string());
      }
    }
  }
}

void Reg_allocator::print_next_var_use(Three_addr_var &var, std::optional<int> next_use)
{
  return;
  if (!var.is_const())
  {
    std::string next_use_str = next_use ? " used at index " + std::to_string(next_use.value()) : " not used again";
    m_debug_log << "Variable " << var.to_string() << next_use_str << std::endl;
  }
}

Three_addr_var &Reg_allocator::reg_entry_var(register_entry &reg_entry)
{
  return std::get<0>(reg_entry);
}

x86_Register &Reg_allocator::reg_entry_reg(register_entry &reg_entry)
{
  return std::get<1>(reg_entry);
}

std::optional<int> &Reg_allocator::reg_entry_dist(register_entry &reg_entry)
{
  return std::get<2>(reg_entry);
}

void Reg_allocator::reset_cfg_data_members()
{
  m_start_index_to_graph_index.clear();
  m_cfg_graph.clear();
}

/*  This function needs to clear m_allocated_reg_data, m_register_free_status, and reset m_free_reg_stack */
void Reg_allocator::reset_registers()
{
  /*  resets m_allocated_reg_data */
  for (auto &entry : m_allocated_reg_data)
  {
    reg_entry_dist(entry) = std::nullopt;
    reg_entry_var(entry) = Three_addr_var();
  }

  /*  Resets m_register_free_status */
  m_register_free_status = std::vector<bool>(m_last_reg_index + 1, true);

  /*  Add all registers to free register stack */
  m_free_reg_stack = {};

  for (int i = 0; i <= m_last_reg_index; i++)
  {
    m_free_reg_stack.push(static_cast<x86_Register>(i));
  }
}

/*  
  Iteratres through m_three_addr_code and adds any basic blocks found to m_cfg_graph

  Basic block defintion:
      Basic block starts:     at a label OR after a jump instruction 
      Basic block ends:       at a jump instruction (jump IS included in the basic block) OR before a 
                                label instruction (label is NOT included in the basic block) OR at 
                                return statement

  Basic blocks have [0,2] children:
      return statements/last instruction in the IR code have 0 children
      unconditional jumps have 1 child (the jump target) 
      conditional jumps have 2 targets (the jump target and the fall through target)
*/
void Reg_allocator::generate_CFG(std::vector<three_addr_code_entry> &IR_code)
{

  std::unordered_map<std::string, int> label_to_index_map;

  /*  add all label names and corresponding indicies to the table */
  for (int i = 0; i < IR_code.size(); i++)
  {
    if (std::get<1>(IR_code.at(i)) == Three_addr_OP::LABEL)
    {
      label_to_index_map.insert({std::get<2>(IR_code.at(i)).to_string(), i});
    }
  }

  /*  create basic blocks out of IR_code */
  int start_of_current_block;
  for (int cur_index = 0; cur_index < IR_code.size(); cur_index++)
  {
    Three_addr_OP cur_inst = std::get<1>(IR_code.at(cur_index));

    /*  
        End at:
            1)  if current instruction is jmp
            2)  if current instruction is a return statement
            3)  if next instruction is label
            4)  if at end of three_addr_code
    */
    if (cur_inst == Three_addr_OP::UNCOND_J)
    {
      int jmp_target = label_to_index_map.at(std::get<2>(IR_code.at(cur_index)).to_string());
      m_cfg_graph.push_back({CFG_node(start_of_current_block, cur_index, jmp_target), false});
      m_start_index_to_graph_index.insert({start_of_current_block, m_cfg_graph.size() - 1});
      start_of_current_block = cur_index + 1;
    }
    else if (cur_inst == Three_addr_OP::EQUAL_J || cur_inst == Three_addr_OP::NEQUAL_J)
    {
      int fall_through_target = cur_index + 1;
      int jmp_target = label_to_index_map.at(std::get<2>(IR_code.at(cur_index)).to_string());
      m_cfg_graph.push_back({CFG_node(start_of_current_block, cur_index, jmp_target, fall_through_target), false});
      m_start_index_to_graph_index.insert({start_of_current_block, m_cfg_graph.size() - 1});
      start_of_current_block = cur_index + 1;
    }
    else if (std::get<1>(IR_code.at(cur_index)) == Three_addr_OP::RET)
    {
      m_cfg_graph.push_back({CFG_node(start_of_current_block, cur_index), false});
      m_start_index_to_graph_index.insert({start_of_current_block, m_cfg_graph.size() - 1});
      start_of_current_block = cur_index + 1;
    }
    else if (cur_index != IR_code.size() - 1 &&
             std::get<1>(IR_code.at(cur_index + 1)) == Three_addr_OP::LABEL)
    {
      m_cfg_graph.push_back({CFG_node(start_of_current_block, cur_index, cur_index + 1), false});
      m_start_index_to_graph_index.insert({start_of_current_block, m_cfg_graph.size() - 1});
    }
    else if (cur_index == IR_code.size() - 1)
    {
      m_cfg_graph.push_back({CFG_node(start_of_current_block, cur_index), false});
      m_start_index_to_graph_index.insert({start_of_current_block, m_cfg_graph.size() - 1});
    }
    /*
      Start at:
          1)  if current instruction is a label
    */
    else if (cur_inst == Three_addr_OP::LABEL)
    {
      start_of_current_block = cur_index;
    }
  }
}

/*  Calculates the live out set for each cfg node in m_cfg_graph. */
void Reg_allocator::generate_live_out_overall(std::vector<three_addr_code_entry> &IR_code)
{
  generate_varkill_uevar(IR_code);

  bool live_out_changed = true;

  /* Iterate over every cfg node until all live_out sets stop changing */
  while (live_out_changed)
  {
    live_out_changed = false;

    for (auto &cur_cfg_entry : m_cfg_graph)
    {
      live_out_changed |= generate_live_out_from_node(cur_cfg_entry.first);
    }
  }
}

bool Reg_allocator::generate_live_out_from_node(CFG_node &node)
{
  std::vector<int> start_indicies_child_nodes;

  /*  Add child nodes to current CFG node */
  if (node.m_jmp_target.has_value())
  {
    start_indicies_child_nodes.push_back(node.m_jmp_target.value());
  }

  /*  Add child nodes to current CFG node */
  if (node.m_fall_through_target.has_value())
  {
    start_indicies_child_nodes.push_back(node.m_fall_through_target.value());
  }

  /*  Save old liveout size to see if the size changes after we recompute live_out */
  int prev_live_out_size = node.m_live_out.size();

  for (int cur_index : start_indicies_child_nodes)
  {
    CFG_node &child_node = m_cfg_graph.at(m_start_index_to_graph_index.at(cur_index)).first;

    /*  Add all variables in the child UEVar set to the parent's live out set */
    node.m_live_out.insert(child_node.m_uevar.begin(), child_node.m_uevar.end());

    /*  
        Add all variables that are defined in the child's liveout set but not in the child's varkill set
        to the live out set of the parent node
    */
    for (const std::string &cur_var : child_node.m_live_out)
    {
      if (child_node.m_var_kill.count(cur_var) == 0)
      {
        node.m_live_out.insert(cur_var);
      }
    }
  }

  return node.m_live_out.size() != prev_live_out_size;
}

void Reg_allocator::generate_asm_line(std::optional<x86_Register> result_reg,
                                      Three_addr_OP op,
                                      std::optional<x86_Register> reg_1,
                                      std::optional<x86_Register> reg_2,
                                      std::string jmp_target)
{

  std::vector<std::string> asm_vec;

  CFG_node tmp_node(0, 0);

  auto write_to_file = [&](std::ofstream &file, std::vector<std::string> &asm_list, std::string delim = "\t")
  {
    for (const std::string &str : asm_list)
    {
      file << delim << str << std::endl;
    }
  };

  auto write_cmp_val = [this](std::string custom_cmp,
                              std::vector<std::string> &asm_list,
                              std::optional<x86_Register> reg_1,
                              std::optional<x86_Register> reg_2,
                              std::optional<x86_Register> result_reg)
  {
    asm_list.push_back("xor %r15, %r15");
    asm_list.push_back("mov " + x86_Register_to_string(reg_1.value()) + ", %rax");
    asm_list.push_back("cmp " + x86_Register_to_string(reg_2.value()) + ", %rax");
    asm_list.push_back(custom_cmp);
    asm_list.push_back("mov %r15, " + x86_Register_to_string(result_reg.value()));
  };

  std::optional<x86_Register> reg_1_div = reg_1;
  std::optional<x86_Register> reg_2_div = reg_2;

  switch (op)
  {
  case Three_addr_OP::RETURN_VAL:
    asm_vec.push_back("mov %rax," + x86_Register_to_string(result_reg.value()));
    break;
  case Three_addr_OP::SAVE_REGS:
    mark_in_use_regs();
    for (x86_Register cur_reg : m_general_saved_regs)
    {
      asm_vec.push_back("push " + x86_Register_to_string(cur_reg));
    }
    break;
  case Three_addr_OP::RESTORE_REGS:
    /*  General regs are saved last, so they need to be restored first */
    for (auto rev_it = m_general_saved_regs.rbegin(); rev_it != m_general_saved_regs.rend(); rev_it++)
    {
      asm_vec.push_back("pop " + x86_Register_to_string(*rev_it));
    }
    /*  Register args are saved first, so restored last */
    for (auto rev_it = m_caller_saved_regs.rbegin(); rev_it != m_caller_saved_regs.rend(); rev_it++)
    {
      asm_vec.push_back("pop " + x86_Register_to_string(*rev_it));
    }
    m_caller_saved_regs.clear();
    m_general_saved_regs.clear();
    m_calling_vec_index = 0;
    break;
  case Three_addr_OP::LOAD_ARG:
    /*  
        If the register required for the next function argument is in use, need to push that register onto 
        the stack, then move the value from reg_1 to the calling convention register. If reg_1 is the correct 
        register specified by the calling convention, instead push reg_1 and increment m_calling_vec_index.
    */
    if (reg_1.value() == m_calling_convention_regs.at(m_calling_vec_index))
    {
      asm_vec.push_back("push " + x86_Register_to_string(reg_1.value()));
      m_caller_saved_regs.push_back(reg_1.value());
      m_calling_vec_index++;
      break;
    }

    if (!m_register_free_status.at(static_cast<int>(m_calling_convention_regs.at(m_calling_vec_index))))
    {
      asm_vec.push_back("push " + x86_Register_to_string(m_calling_convention_regs.at(m_calling_vec_index)));
      m_caller_saved_regs.push_back(m_calling_convention_regs.at(m_calling_vec_index));
    }
    asm_vec.push_back(
        "mov " + x86_Register_to_string(reg_1.value()) +
        ", " + x86_Register_to_string(m_calling_convention_regs.at(m_calling_vec_index)));
    m_calling_vec_index++;
    break;
  case Three_addr_OP::CALL:
    asm_vec.push_back("push %rbp");
    asm_vec.push_back("call " + jmp_target);
    asm_vec.push_back("pop %rbp");
    break;
  case Three_addr_OP::RAW_STR:
    asm_vec.push_back(jmp_target);
    break;
  case Three_addr_OP::LOAD:
    asm_vec.push_back("mov (" + x86_Register_to_string(reg_1.value()) + "), " + x86_Register_to_string(result_reg.value()));
    break;
  case Three_addr_OP::STORE:
    asm_vec.push_back("mov " + x86_Register_to_string(reg_2.value()) + ", (" + x86_Register_to_string(reg_1.value()) + ")");
    break;
  case Three_addr_OP::ADD:
    asm_vec.push_back("mov " + x86_Register_to_string(reg_1.value()) + ", " + x86_Register_to_string(result_reg.value()));
    asm_vec.push_back("add " + x86_Register_to_string(reg_2.value()) + ", " + x86_Register_to_string(result_reg.value()));
    break;
  case Three_addr_OP::SUB:
    asm_vec.push_back("mov " + x86_Register_to_string(reg_1.value()) + ", " + x86_Register_to_string(result_reg.value()));
    asm_vec.push_back("sub " + x86_Register_to_string(reg_2.value()) + ", " + x86_Register_to_string(result_reg.value()));
    break;
  case Three_addr_OP::MULT:
    asm_vec.push_back("mov " + x86_Register_to_string(reg_1.value()) + ", " + x86_Register_to_string(result_reg.value()));
    asm_vec.push_back("imul " + x86_Register_to_string(reg_2.value()) + ", " + x86_Register_to_string(result_reg.value()));
    break;
  case Three_addr_OP::DIVIDE:
    if (reg_1.value() == reg_2.value())
    {
      asm_vec.push_back("mov $1," + x86_Register_to_string(result_reg.value()));
      break;
    }
    else if (reg_1.value() == x86_Register::RDX)
    {
      reg_1_div = allocate_reg(Three_addr_var());
      asm_vec.push_back("mov " + x86_Register_to_string(reg_1.value()) + ", " + x86_Register_to_string(reg_1_div.value()));
      /*  This will always free the variable, since the allocated register is empty */
      free(reg_1_div.value(), tmp_node);
    }
    else if (reg_2.value() == x86_Register::RDX)
    {
      reg_2_div = allocate_reg(Three_addr_var());
      asm_vec.push_back("mov " + x86_Register_to_string(reg_2.value()) + ", " + x86_Register_to_string(reg_2_div.value()));
      /*  This will always free the variable, since the allocated register is empty */
      free(reg_2_div.value(), tmp_node);
    }
    else if (!m_register_free_status.at(m_last_reg_index) && result_reg.value() != x86_Register::RDX)
    {
      asm_vec.push_back("push %rdx");
    }

    asm_vec.push_back("xor %rdx, %rdx");
    asm_vec.push_back("mov " + x86_Register_to_string(reg_1_div.value()) + ", " + "%rax");
    asm_vec.push_back("idiv " + x86_Register_to_string(reg_2_div.value()));
    asm_vec.push_back("mov %rax, " + x86_Register_to_string(result_reg.value()));

    if (reg_1.value() != reg_1_div)
    {
      asm_vec.push_back("mov " + x86_Register_to_string(reg_1_div.value()) + ", " + x86_Register_to_string(reg_1.value()));
    }
    else if (reg_2.value() != reg_2_div)
    {
      asm_vec.push_back("mov " + x86_Register_to_string(reg_2_div.value()) + ", " + x86_Register_to_string(reg_2.value()));
    }
    else if (!m_register_free_status.at(m_last_reg_index) && result_reg.value() != x86_Register::RDX)
    {
      asm_vec.push_back("pop %rdx");
    }
    break;
  case Three_addr_OP::RET:
    if (reg_1.has_value())
    {
      asm_vec.push_back("mov " + x86_Register_to_string(reg_1.value()) + ", " + "%rax");
    }
    asm_vec.push_back("ret");
    break;
  case Three_addr_OP::BIT_AND:
    asm_vec.push_back("mov " + x86_Register_to_string(reg_1.value()) + ", " + x86_Register_to_string(result_reg.value()));
    asm_vec.push_back("and " + x86_Register_to_string(reg_2.value()) + ", " + x86_Register_to_string(result_reg.value()));
    break;
  case Three_addr_OP::BIT_OR:
    asm_vec.push_back("mov " + x86_Register_to_string(reg_1.value()) + ", " + x86_Register_to_string(result_reg.value()));
    asm_vec.push_back("or " + x86_Register_to_string(reg_2.value()) + ", " + x86_Register_to_string(result_reg.value()));
    break;
  case Three_addr_OP::ASSIGN:
    if (reg_1.value() != result_reg.value())
    {
      asm_vec.push_back("mov " + x86_Register_to_string(reg_1.value()) + ", " + x86_Register_to_string(result_reg.value()));
    }
    break;
  case Three_addr_OP::CMP:
    asm_vec.push_back("cmp " + x86_Register_to_string(reg_1.value()) + ", " + x86_Register_to_string(reg_2.value()));
    break;
  case Three_addr_OP::UNCOND_J:
    asm_vec.push_back("jmp " + jmp_target);
    break;
  case Three_addr_OP::EQUAL_J:
    asm_vec.push_back("je " + jmp_target);
    break;
  case Three_addr_OP::NEQUAL_J:
    asm_vec.push_back("jne " + jmp_target);
    break;
  /*  Treat func names the same as labels */
  case Three_addr_OP::LABEL:
    asm_vec.push_back(jmp_target + ":");
    break;
  case Three_addr_OP::EQUALITY:
    write_cmp_val("sete %r15b", asm_vec, reg_1, reg_2, result_reg);
    break;
  case Three_addr_OP::NOT_EQUALITY:
    write_cmp_val("setne %r15b", asm_vec, reg_1, reg_2, result_reg);
    break;
  case Three_addr_OP::LESS_THAN:
    write_cmp_val("setl %r15b", asm_vec, reg_1, reg_2, result_reg);
    break;
  case Three_addr_OP::LESS_THAN_EQUAL:
    write_cmp_val("setle %r15b", asm_vec, reg_1, reg_2, result_reg);
    break;
  case Three_addr_OP::GREATER_THAN:
    write_cmp_val("setg %r15b", asm_vec, reg_1, reg_2, result_reg);
    break;
  case Three_addr_OP::GREATER_THAN_EQUAL:
    write_cmp_val("setge %r15b", asm_vec, reg_1, reg_2, result_reg);
    break;
  default:
    asm_vec.push_back("Error, invalid op code passed to generate_asm_line ");
    break;
  }
  write_to_file(m_asm_file, asm_vec);
}

void Reg_allocator::generate_assembly_from_CFG_node(CFG_node &node,
                                                    std::vector<three_addr_code_entry> &IR_code)
{
  for (int i = node.m_start_index; i <= node.m_end_index; i++)
  {
    Three_addr_var var_result = std::get<0>(IR_code.at(i));
    Three_addr_OP cur_op = std::get<1>(IR_code.at(i));
    Three_addr_var var_1 = std::get<2>(IR_code.at(i));
    Three_addr_var var_2 = std::get<3>(IR_code.at(i));

    std::optional<x86_Register> reg_1;
    std::optional<x86_Register> reg_2;
    std::optional<x86_Register> reg_result;

    std::string jmp_target = "";

    if (var_1.is_valid())
    {
      reg_1 = ensure(var_1);
      jmp_target = var_1.to_string();
    }

    if (var_2.is_valid())
    {
      reg_2 = ensure(var_2);
    }

    if (var_result.is_valid())
    {
      std::optional<x86_Register> allocated_var = find_allocated_var(var_result);
      if (allocated_var.has_value())
      {
        reg_result = allocated_var;
      }
      else
      {
        reg_result = allocate_reg(var_result);
      }
    }

    /*  
        Variables need to be saved to memory before the jmp instruction is taken, and save_live_out_vars()
        is called at the end of the loop, which would be too late for memory to be saved
    */
    if (cur_op == Three_addr_OP::UNCOND_J || cur_op == Three_addr_OP::NEQUAL_J || cur_op == Three_addr_OP::EQUAL_J)
    {
      save_live_out_vars(node);
    }

    generate_asm_line(reg_result, cur_op, reg_1, reg_2, jmp_target);

    /*  
        Frees the register if it holds a variable that isn't being used again in the future. Otherwise sets the next use of the 
        variable to the index at which it occurs next 
    */
    auto update_or_free_reg = [this](std::optional<x86_Register> &reg, Three_addr_var &var,
                                     CFG_node &node, int start_index, std::vector<three_addr_code_entry> &IR_code)
    {
      if (reg.has_value())
      {
        std::optional<int> next_var_use = dist_to_next_var_occurance(var, start_index + 1, node, IR_code);
        if (next_var_use.has_value())
        {
          reg_entry_dist(m_allocated_reg_data[static_cast<int>(reg.value())]) = next_var_use.value();
        }
        else
        {
          free(reg.value(), node);
        }
      }
    };

    update_or_free_reg(reg_1, var_1, node, i, IR_code);
    update_or_free_reg(reg_2, var_2, node, i, IR_code);
    update_or_free_reg(reg_result, var_result, node, i, IR_code);

    print_register_contents();
  }
  save_live_out_vars(node);
}

void Reg_allocator::save_live_out_vars(CFG_node &node)
{
  for (const std::string &cur_live_out : node.m_live_out)
  {
    for (register_entry &cur_reg_entry : m_allocated_reg_data)
    {
      /*  
          The only time we don't need to save a variable to liveout is if it's an array variable that's 
          local to the current function, since array values are always written to memory directly
      */
      if (reg_entry_var(cur_reg_entry).is_var() && reg_entry_var(cur_reg_entry).to_string() == cur_live_out)
      {
        store_reg(reg_entry_reg(cur_reg_entry));
      }
    }
  }
  /*  
      Each node is only processed once. Clearing the node at the end prevents the variables from being
      saved to memory again once generate_assembly_from_CFG finishes running
  */
  node.m_live_out.clear();
}

std::optional<x86_Register> Reg_allocator::ensure(const Three_addr_var &var_to_be_allocated)
{
  /* If the var_to_be_allocated doesn't need a register, return an emtpy optional */
  if (var_to_be_allocated.is_raw_str() || var_to_be_allocated.is_label())
  {
    return std::nullopt;
  }

  std::optional<x86_Register> allocated_value = find_allocated_var(var_to_be_allocated);

  if (allocated_value.has_value())
  {
    return allocated_value;
  }

  /*  Allocate register if the value we need isn't contained inside a register */
  x86_Register newly_freed_register = allocate_reg(var_to_be_allocated);

  /*  Loads value from var_to_be_allocated into newly_freed_register */
  load_reg(var_to_be_allocated, newly_freed_register);

  return newly_freed_register;
}

void Reg_allocator::load_reg(const Three_addr_var &var_to_be_allocated, x86_Register free_reg)
{

  /*  Load variable into register */
  if (var_to_be_allocated.is_scalar_var())
  {
    std::string source_reg_str = x86_Register_to_string(free_reg);
    std::string var_offset_str = std::to_string(get_var_offset_cond_add(var_to_be_allocated.to_string()));
    m_asm_file << "\tmov " << var_offset_str << "(%rbp), " << source_reg_str << std::endl;
    m_debug_log << "loading variable from offset " << var_offset_str << " into register " << source_reg_str << std::endl;
  }
  else if (var_to_be_allocated.is_array())
  {
    std::vector<std::string> func_args = m_prog_sym_table.get_cur_func_args();

    /*  If array is a function arg, load base addr from memory into register */
    if (find(func_args.begin(), func_args.end(), var_to_be_allocated.to_string()) != func_args.end())
    {
      std::string source_reg_str = x86_Register_to_string(free_reg);
      std::string var_offset_str = std::to_string(get_var_offset_cond_add(var_to_be_allocated.to_string()));
      m_asm_file << "\tmov " << var_offset_str << "(%rbp), " << source_reg_str << std::endl;
    }
    /*  If array isn't a function arg, calculate base addr using offset from %rbp */
    else
    {
      std::string var_offset_str =
          std::to_string(m_prog_sym_table.get_var_offset(var_to_be_allocated.to_string()).value());
      m_asm_file << "\tmov %rbp, " << x86_Register_to_string(free_reg) << std::endl;
      m_asm_file << "\tadd $" << var_offset_str << ", " << x86_Register_to_string(free_reg) << std::endl;
    }
  }
  else
  {
    m_asm_file << "\tmov $" << var_to_be_allocated.to_string() << ", " << x86_Register_to_string(free_reg)
               << std::endl;
  }
}

x86_Register Reg_allocator::allocate_reg(const Three_addr_var &var_to_be_allocated)
{
  if (m_free_reg_stack.size() > 0)
  {
    x86_Register free_reg = m_free_reg_stack.top();
    m_free_reg_stack.pop();

    int reg_as_index = static_cast<int>(free_reg);

    reg_entry_var(m_allocated_reg_data.at(reg_as_index)) = var_to_be_allocated;
    reg_entry_dist(m_allocated_reg_data.at(reg_as_index)) = std::nullopt;
    m_register_free_status.at(reg_as_index) = false;

    return free_reg;
  }
  else
  {
    /*  Get register that is used the furthest in the future */
    int next_furthest_use = 0;
    int next_furthest_use_index = 0;

    for (int i = 0; i < m_allocated_reg_data.size(); i++)
    {
      register_entry &cur_entry = m_allocated_reg_data.at(i);
      std::optional<int> next_reg_use = reg_entry_dist(cur_entry);

      if (next_reg_use.has_value() && (next_reg_use.value() > next_furthest_use))
      {
        next_furthest_use = next_reg_use.value();
        next_furthest_use_index = i;
      }
    }

    register_entry &reg_entry_being_freed = m_allocated_reg_data.at(next_furthest_use_index);

    /*  
        Only write to memory if register has a temporary variable that is used later in this block. Arrays
        are always written to memory directly, so no need to write an array being freed to memory
    */
    if (reg_entry_var(reg_entry_being_freed).is_scalar_var())
    {
      store_reg(reg_entry_reg(reg_entry_being_freed));
    }

    reg_entry_var(m_allocated_reg_data.at(next_furthest_use_index)) = var_to_be_allocated;

    /*  
        Set distance to next instruction to nullopt to ensure that this register 
        isn't deallocated for another variable in the same intruction
    */
    reg_entry_dist(m_allocated_reg_data.at(next_furthest_use_index)) = std::nullopt;
    m_register_free_status.at(next_furthest_use_index) = false;

    return static_cast<x86_Register>(next_furthest_use_index);
  }
}

void Reg_allocator::free(x86_Register reg_to_free, CFG_node &node)
{

  int reg_index = static_cast<int>(reg_to_free);

  /*  Only free register if it hasn't been freed yet  */
  if (!m_register_free_status.at(reg_index))
  {
    m_free_reg_stack.push(reg_to_free);

    /* Write variable in register to memory if that variable is in the live out set of node */
    Three_addr_var &var_being_freed = reg_entry_var(m_allocated_reg_data.at(reg_index));
    if (var_being_freed.is_scalar_var() && node.m_live_out.count(var_being_freed.to_string()) != 0)
    {
      store_reg(reg_to_free);
    }

    reg_entry_var(m_allocated_reg_data.at(reg_index)) = Three_addr_var();
    reg_entry_dist(m_allocated_reg_data.at(reg_index)) = std::nullopt;
    m_register_free_status[reg_index] = true;
  }
}

void Reg_allocator::store_reg(x86_Register reg_to_free)
{
  std::string source_reg_str = x86_Register_to_string(reg_to_free);
  std::string var_name = std::get<0>(m_allocated_reg_data.at(static_cast<int>(reg_to_free))).to_string();
  std::string var_offset_str = std::to_string(get_var_offset_cond_add(var_name));
  m_asm_file << "\tmov " << source_reg_str << ", " << var_offset_str << "(%rbp)" << std::endl;
  m_debug_log << "storing variable " << var_name << " in live out set with offset " << var_offset_str
              << std::endl;
}

std::optional<x86_Register> Reg_allocator::find_allocated_var(const Three_addr_var &var_to_be_allocated)
{
  for (register_entry &entry : m_allocated_reg_data)
  {
    if (reg_entry_var(entry) == var_to_be_allocated)
    {
      return reg_entry_reg(entry);
    }
  }
  return std::nullopt;
}

std::optional<int> Reg_allocator::dist_to_next_var_occurance(const Three_addr_var &search_var,
                                                             int start_index,
                                                             const CFG_node &node,
                                                             const std::vector<three_addr_code_entry> &IR_code)
{
  for (int cur_index = start_index; cur_index <= node.m_end_index; cur_index++)
  {

    const three_addr_code_entry &cur_entry = IR_code.at(cur_index);

    if (std::get<0>(cur_entry) == search_var)
    {
      return cur_index;
    }

    if (std::get<2>(cur_entry) == search_var)
    {
      return cur_index;
    }

    if (std::get<3>(cur_entry) == search_var)
    {
      return cur_index;
    }
  }
  return std::nullopt;
}

int Reg_allocator::get_var_offset_cond_add(std::string var_name)
{
  std::optional<int> var_offset = m_prog_sym_table.get_var_offset(var_name);

  if (!var_offset)
  {
    m_prog_sym_table.add_local_var(var_name, nullptr);
    return m_prog_sym_table.get_var_offset(var_name).value();
  }
  return var_offset.value();
}
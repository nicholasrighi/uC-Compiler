#include "Reg_allocator.h"

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
  case x86_Register::r15:
    return "%r15";
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
  default:
    return "Error, invalid register name passed to x86_Register_to_string function";
    break;
  }
}

Reg_allocator::Reg_allocator(std::string asm_file_name, std::ofstream &debug_log,
                             Program_symbol_table &sym_table, std::vector<three_addr_code_entry> &three_addr_code)
    : m_prog_sym_table(sym_table), m_three_addr_code(three_addr_code), m_debug_log(debug_log)
{
  m_asm_file.open(asm_file_name, std::ofstream::trunc);

  m_asm_file << ".globl main" << std::endl;

  for (int i = 0; i < static_cast<int>(x86_Register::RDI); i++)
  {
    x86_Register cur_reg = static_cast<x86_Register>(i);
    m_allocated_reg_data.push_back(std::make_tuple(Three_addr_var(), cur_reg, std::nullopt));
    m_register_free_status.push_back(true);
    m_free_reg_stack.push(cur_reg);
  }
}

Reg_allocator::~Reg_allocator()
{
  m_asm_file.close();
}

/*  
  This is stubbed out right now, needs to be fixed later. Needs to call generate_CFG(), then 
  create liveout set, then loop through all nodes in the CFG and generate assembly for each node.
  Currently only generates assembly for a single block of the CFG, and the CFG functions aren't complete
  yet
*/
void Reg_allocator::generate_asm_file()
{
  m_debug_log << "\nStart of debug log for assembly\n"
              << std::endl;

  generate_CFG();
  generate_live_out_overall();
  print_CFG();
  for (auto &CFG_node : m_cfg_graph)
  {
    if (!CFG_node.second)
    {
      generate_assembly_from_CFG_node(CFG_node.first);
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

/*  
  Assumes all operations are of the form "x <- y op z", where y and z are optional
*/
void Reg_allocator::generate_varkill_uevar()
{
  for (CFG_entry &cur_entry : m_cfg_graph)
  {
    CFG_node &node = cur_entry.first;
    for (int i = node.m_start_index; i <= node.m_end_index; i++)
    {
      Three_addr_var x = std::get<0>(m_three_addr_code[i]);
      Three_addr_var y = std::get<2>(m_three_addr_code[i]);
      Three_addr_var z = std::get<3>(m_three_addr_code[i]);

      /*  If a variable is defined in a basic block then it's added to the varkill set */
      if (x.is_valid() && !x.is_raw_str() && !x.is_label())
      {
        node.m_var_kill.insert(x.to_string());
      }

      /*  
          If variables y and z are used but aren't in varkill, they must be defined in an earlier
          basic block. This means they need to be added to ueavar 
      */
      if (y.is_valid() && y.is_string() && (node.m_var_kill.count(y.to_string()) == 0))
      {
        node.m_uevar.insert(y.to_string());
      }

      if (z.is_valid() && z.is_string() && (node.m_var_kill.count(z.to_string()) == 0))
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

/*  
  Iteratres through m_three_addr_code and locates basic blocks. Adds any basic blocks found to the 
  m_cfg_graph. 

  Basic block defintion:
      Basic block starts at:  label OR after a jump instruction
      Basic block ends at:    at a jump instruction (jump IS included in the basic block) OR before a 
                                label instruction (label is NOT included in the basic block)

  Each basic block has either [0,2] children:
      return statements/last instruction in the list have 0 children
      unconditional jumps have 1 child (the jump target) 
      conditional jumps have 2 targets (the jump target and the fall through target)
*/
void Reg_allocator::generate_CFG()
{

  std::unordered_map<std::string, int> label_to_index_map;

  /*  add all label names and corresponding indicies to the table */
  for (int i = 0; i < m_three_addr_code.size(); i++)
  {
    if (std::get<1>(m_three_addr_code.at(i)) == Three_addr_OP::LABEL)
    {
      label_to_index_map.insert({std::get<2>(m_three_addr_code.at(i)).to_string(), i});
    }
  }

  /*  create basic blocks out of m_three_addr_code */
  int start_of_current_block;
  for (int cur_index = 0; cur_index < m_three_addr_code.size(); cur_index++)
  {
    Three_addr_OP cur_inst = std::get<1>(m_three_addr_code.at(cur_index));

    /*  Basic blocks end at the beginning of a jump instruction */
    if (cur_inst == Three_addr_OP::UNCOND_J)
    {
      int jmp_target = label_to_index_map.at(std::get<2>(m_three_addr_code.at(cur_index)).to_string());
      m_cfg_graph.push_back({CFG_node(start_of_current_block, cur_index, jmp_target), false});
      m_start_index_to_graph_index.insert({start_of_current_block, m_cfg_graph.size() - 1});
    }
    else if (cur_inst == Three_addr_OP::EQUAL_J)
    {
      int fall_through_target = cur_index + 1;
      int jmp_target = label_to_index_map.at(std::get<2>(m_three_addr_code.at(cur_index)).to_string());
      m_cfg_graph.push_back({CFG_node(start_of_current_block, cur_index, jmp_target, fall_through_target), false});
      m_start_index_to_graph_index.insert({start_of_current_block, m_cfg_graph.size() - 1});
    }
    /*  Basic blocks start at a label */
    else if (cur_inst == Three_addr_OP::LABEL)
    {
      start_of_current_block = cur_index;
    }
    /*  Basic blocks start after a jump instruction */
    else if (cur_index != 0 && (std::get<1>(m_three_addr_code.at(cur_index - 1)) == Three_addr_OP::UNCOND_J ||
                                std::get<1>(m_three_addr_code.at(cur_index - 1)) == Three_addr_OP::EQUAL_J))
    {
      start_of_current_block = cur_index;
    }
    /*  Basic blocks end before a label */
    else if (cur_index != m_three_addr_code.size() - 1 &&
             std::get<1>(m_three_addr_code.at(cur_index + 1)) == Three_addr_OP::LABEL)
    {
      m_cfg_graph.push_back({CFG_node(start_of_current_block, cur_index, cur_index + 1), false});
      m_start_index_to_graph_index.insert({start_of_current_block, m_cfg_graph.size() - 1});
    }
    /*  End current block if this is the last instruction in the list */
    else if (cur_index == m_three_addr_code.size() - 1)
    {
      m_cfg_graph.push_back({CFG_node(start_of_current_block, cur_index), false});
      m_start_index_to_graph_index.insert({start_of_current_block, m_cfg_graph.size() - 1});
    }
  }
}

/*  Calculates the live out set for each cfg node in m_cfg_graph. */
void Reg_allocator::generate_live_out_overall()
{
  generate_varkill_uevar();

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

  if (node.m_jmp_target.has_value())
  {
    start_indicies_child_nodes.push_back(node.m_jmp_target.value());
  }

  if (node.m_fall_through_target.has_value())
  {
    start_indicies_child_nodes.push_back(node.m_fall_through_target.value());
  }

  /* Recalculate child nodes liveout */
  for (int cur_index : start_indicies_child_nodes)
  {
    CFG_node &child_node = m_cfg_graph.at(m_start_index_to_graph_index.at(cur_index)).first;
    generate_live_out_from_node(child_node);
  }

  /*  Save old liveout size to see if the size changes after we recompute live_out */
  int prev_live_out_size = node.m_live_out.size();

  for (int cur_index : start_indicies_child_nodes)
  {
    CFG_node &child_node = m_cfg_graph.at(m_start_index_to_graph_index.at(cur_index)).first;

    /*  Add all variables in the child UEVar set to the parent's live out set */
    node.m_live_out.insert(child_node.m_uevar.begin(), child_node.m_uevar.end());

    /*  
        Add all variables in the child's live out set that are in the child's live out set but aren't defined
        in the child's basic block
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

void Reg_allocator::generate_asm_line(std::optional<x86_Register> result_reg, Three_addr_OP op,
                                      std::optional<x86_Register> reg_1, std::optional<x86_Register> reg_2,
                                      std::string jmp_target)
{

  std::vector<std::string> asm_vec;

  auto write_to_file = [&](std::ofstream &file, std::vector<std::string> &asm_list, std::string delim = "\t")
  {
    for (const std::string &str : asm_list)
    {
      file << delim << str << std::endl;
    }
  };

  auto get_var_from_reg = [this](x86_Register reg)
  {
    return reg_entry_var(m_allocated_reg_data[static_cast<int>(reg)]).to_string();
  };

  switch (op)
  {
  case Three_addr_OP::RAW_STR:
    asm_vec.push_back(jmp_target);
    write_to_file(m_asm_file, asm_vec);
    break;
  case Three_addr_OP::LOAD:
    asm_vec.push_back("mov " + x86_Register_to_string(reg_1.value()) + ", " + x86_Register_to_string(result_reg.value()));
    write_to_file(m_asm_file, asm_vec);
    break;
  case Three_addr_OP::STORE:
    m_debug_log << "Storing value from " + x86_Register_to_string(reg_1.value()) + " into " + x86_Register_to_string(result_reg.value()) << std::endl;
    break;
  case Three_addr_OP::ADD:
    asm_vec.push_back("mov " + x86_Register_to_string(reg_1.value()) + ", " + x86_Register_to_string(result_reg.value()));
    asm_vec.push_back("add " + x86_Register_to_string(reg_2.value()) + ", " + x86_Register_to_string(result_reg.value()));
    write_to_file(m_asm_file, asm_vec);
    break;
  case Three_addr_OP::SUB:
    asm_vec.push_back("mov " + x86_Register_to_string(reg_1.value()) + ", " + x86_Register_to_string(result_reg.value()));
    asm_vec.push_back("sub " + x86_Register_to_string(reg_2.value()) + ", " + x86_Register_to_string(result_reg.value()));
    write_to_file(m_asm_file, asm_vec);
    break;
  case Three_addr_OP::MULT:
    asm_vec.push_back("mov " + x86_Register_to_string(reg_1.value()) + ", " + x86_Register_to_string(result_reg.value()));
    asm_vec.push_back("imul " + x86_Register_to_string(reg_2.value()) + ", " + x86_Register_to_string(result_reg.value()));
    write_to_file(m_asm_file, asm_vec);
    break;
  case Three_addr_OP::DIVIDE:
    asm_vec.push_back("mov " + x86_Register_to_string(reg_1.value()) + ", " + "%rax");
    asm_vec.push_back("xor %rdx, %rdx");
    asm_vec.push_back("idiv " + x86_Register_to_string(reg_2.value()));
    asm_vec.push_back("mov %rax, " + x86_Register_to_string(result_reg.value()));
    write_to_file(m_asm_file, asm_vec);
    break;
  case Three_addr_OP::RET:
    asm_vec.push_back("\tmov " + x86_Register_to_string(reg_1.value()) + ", " + "%rax");
    asm_vec.push_back("\tret");
    write_to_file(m_asm_file, asm_vec);
    break;
  case Three_addr_OP::BIT_AND:
    asm_vec.push_back("mov " + x86_Register_to_string(reg_1.value()) + ", " + x86_Register_to_string(result_reg.value()));
    asm_vec.push_back("and " + x86_Register_to_string(reg_2.value()) + ", " + x86_Register_to_string(result_reg.value()));
    write_to_file(m_asm_file, asm_vec);
    break;
  case Three_addr_OP::BIT_OR:
    asm_vec.push_back("mov " + x86_Register_to_string(reg_1.value()) + ", " + x86_Register_to_string(result_reg.value()));
    asm_vec.push_back("or " + x86_Register_to_string(reg_2.value()) + ", " + x86_Register_to_string(result_reg.value()));
    write_to_file(m_asm_file, asm_vec);
    break;
  case Three_addr_OP::ASSIGN:
    if (reg_1.value() != result_reg.value())
    {
      asm_vec.push_back("mov " + x86_Register_to_string(reg_1.value()) + ", " + x86_Register_to_string(result_reg.value()));
      write_to_file(m_asm_file, asm_vec);
    }
    break;
  case Three_addr_OP::CMP:
    asm_vec.push_back("cmp " + x86_Register_to_string(reg_1.value()) + ", " + x86_Register_to_string(reg_2.value()));
    write_to_file(m_asm_file, asm_vec);
    break;
  case Three_addr_OP::UNCOND_J:
    asm_vec.push_back("jmp " + jmp_target);
    write_to_file(m_asm_file, asm_vec);
    break;
  case Three_addr_OP::EQUAL_J:
    asm_vec.push_back("je " + jmp_target);
    write_to_file(m_asm_file, asm_vec);
    break;
  case Three_addr_OP::LABEL:
    asm_vec.push_back(jmp_target + ":");
    write_to_file(m_asm_file, asm_vec);
    break;
  case Three_addr_OP::EQUALITY:
    asm_vec.push_back("xor %rdx, %rdx");
    asm_vec.push_back("mov " + x86_Register_to_string(reg_1.value()) + ", %rax");
    asm_vec.push_back("cmp " + x86_Register_to_string(reg_2.value()) + ", %rax");
    asm_vec.push_back("sete %dl");
    asm_vec.push_back("mov %rdx, " + x86_Register_to_string(result_reg.value()));
    write_to_file(m_asm_file, asm_vec);
    break;
  case Three_addr_OP::NOT_EQUALITY:
    asm_vec.push_back("xor %rdx, %rdx");
    asm_vec.push_back("mov " + x86_Register_to_string(reg_1.value()) + ", %rax");
    asm_vec.push_back("cmp " + x86_Register_to_string(reg_2.value()) + ", %rax");
    asm_vec.push_back("setne %dl");
    asm_vec.push_back("mov %rdx, " + x86_Register_to_string(result_reg.value()));
    write_to_file(m_asm_file, asm_vec);
    break;
  case Three_addr_OP::LESS_THAN:
    asm_vec.push_back("xor %rdx, %rdx");
    asm_vec.push_back("mov " + x86_Register_to_string(reg_1.value()) + ", %rax");
    asm_vec.push_back("cmp " + x86_Register_to_string(reg_2.value()) + ", %rax");
    asm_vec.push_back("setl %dl");
    asm_vec.push_back("mov %rdx, " + x86_Register_to_string(result_reg.value()));
    write_to_file(m_asm_file, asm_vec);
    break;
  case Three_addr_OP::LESS_THAN_EQUAL:
    asm_vec.push_back("xor %rdx, %rdx");
    asm_vec.push_back("mov " + x86_Register_to_string(reg_1.value()) + ", %rax");
    asm_vec.push_back("cmp " + x86_Register_to_string(reg_2.value()) + ", %rax");
    asm_vec.push_back("setle %dl");
    asm_vec.push_back("mov %rdx, " + x86_Register_to_string(result_reg.value()));
    write_to_file(m_asm_file, asm_vec);
    break;
  case Three_addr_OP::GREATER_THAN:
    asm_vec.push_back("xor %rdx, %rdx");
    asm_vec.push_back("mov " + x86_Register_to_string(reg_1.value()) + ", %rax");
    asm_vec.push_back("cmp " + x86_Register_to_string(reg_2.value()) + ", %rax");
    asm_vec.push_back("setg %dl");
    asm_vec.push_back("mov %rdx, " + x86_Register_to_string(result_reg.value()));
    write_to_file(m_asm_file, asm_vec);
    break;
  case Three_addr_OP::GREATER_THAN_EQUAL:
    asm_vec.push_back("xor %rdx, %rdx");
    asm_vec.push_back("mov " + x86_Register_to_string(reg_1.value()) + ", %rax");
    asm_vec.push_back("cmp " + x86_Register_to_string(reg_2.value()) + ", %rax");
    asm_vec.push_back("setge %dl");
    asm_vec.push_back("mov %rdx, " + x86_Register_to_string(result_reg.value()));
    write_to_file(m_asm_file, asm_vec);
    break;
  default:
    m_asm_file << "Error, invalid op code passed to generate_asm_line " << std::endl;
    break;
  }
}

void Reg_allocator::generate_assembly_from_CFG_node(const CFG_node &node)
{
  for (int i = node.m_start_index; i <= node.m_end_index; i++)
  {
    Three_addr_var var_result = std::get<0>(m_three_addr_code[i]);
    Three_addr_OP cur_op = std::get<1>(m_three_addr_code[i]);
    Three_addr_var var_1 = std::get<2>(m_three_addr_code[i]);
    Three_addr_var var_2 = std::get<3>(m_three_addr_code[i]);

    std::optional<x86_Register> reg_1;
    std::optional<x86_Register> reg_2;
    std::optional<x86_Register> reg_result;

    std::string jmp_target = "";

    if (var_1.is_valid())
    {
      reg_1 = ensure(var_1, i + 1, node);
      jmp_target = var_1.to_string();
    }

    if (var_2.is_valid())
    {
      reg_2 = ensure(var_2, i + 1, node);
    }

    if (var_result.is_valid())
    {
      reg_result = allocate_reg(var_result, i + 1, node);
    }

    generate_asm_line(reg_result, cur_op, reg_1, reg_2, jmp_target);

    if (reg_1.has_value())
    {
      std::optional<int> next_var_1_use = dist_to_next_var_occurance(var_1, i + 1, node);
      if (next_var_1_use.has_value())
      {
        reg_entry_dist(m_allocated_reg_data[static_cast<int>(reg_1.value())]) = next_var_1_use.value();
      }
      else
      {
        free(reg_1.value(), node);
      }
      print_next_var_use(var_1, next_var_1_use);
    }

    if (reg_2.has_value())
    {
      std::optional<int> next_var_2_use = dist_to_next_var_occurance(var_2, i + 1, node);
      if (next_var_2_use.has_value())
      {
        reg_entry_dist(m_allocated_reg_data[static_cast<int>(reg_2.value())]) = next_var_2_use.value();
      }
      else
      {
        free(reg_2.value(), node);
      }
      print_next_var_use(var_2, next_var_2_use);
    }

    if (reg_result.has_value())
    {
      std::optional<int> next_var_result_use = dist_to_next_var_occurance(var_result, i + 1, node);
      if (next_var_result_use.has_value())
      {
        reg_entry_dist(m_allocated_reg_data[static_cast<int>(reg_result.value())]) = next_var_result_use.value();
      }
      else
      {
        free(reg_result.value(), node);
      }
      print_next_var_use(var_result, next_var_result_use);
    }
    print_register_contents();
  }
  save_live_out_vars(node);
}

void Reg_allocator::save_live_out_vars(const CFG_node &node)
{

  for (const std::string &cur_live_out : node.m_live_out)
  {
    for (register_entry &cur_reg_entry : m_allocated_reg_data)
    {
      if (reg_entry_var(cur_reg_entry).is_string() && reg_entry_var(cur_reg_entry).to_string() == cur_live_out)
      {
        std::string source_reg_str = x86_Register_to_string(reg_entry_reg(cur_reg_entry));
        std::string var_offset_str = std::to_string(get_var_offset_cond_add(reg_entry_var(cur_reg_entry).to_string()));
        m_asm_file << "\tmov " << source_reg_str << ", -" << var_offset_str << "(%rbp)" << std::endl;
        free(reg_entry_reg(cur_reg_entry), node);
      }
    }
  }
}

std::optional<x86_Register> Reg_allocator::ensure(const Three_addr_var &var_to_be_allocated, int start_index, const CFG_node &node)
{
  /* If the var_to_be_allocated doesn't need a register, return an emtpy optional */
  if (var_to_be_allocated.is_raw_str() || var_to_be_allocated.is_label())
  {
    return std::nullopt;
  }

  /*  If a register already contains the value we're searching for, return that register */
  for (register_entry &entry : m_allocated_reg_data)
  {
    if (reg_entry_var(entry) == var_to_be_allocated)
    {
      return reg_entry_reg(entry);
    }
  }

  /*  Allocate register if the value we need isn't contained inside a register */
  x86_Register newly_freed_register = allocate_reg(var_to_be_allocated, start_index, node);

  /*  Load variable into register */
  if (var_to_be_allocated.is_string())
  {
    std::string source_reg_str = x86_Register_to_string(newly_freed_register);
    std::string var_offset_str = std::to_string(get_var_offset_cond_add(var_to_be_allocated.to_string()));
    m_asm_file << "\tmov -" << var_offset_str << "(%rbp), " << source_reg_str << std::endl;
    m_debug_log << "loading variable from offset " << var_offset_str << " into register " << source_reg_str << std::endl;
  }
  else
  {
    m_asm_file << "\tmov $" << var_to_be_allocated.to_string() << ", "
               << x86_Register_to_string(newly_freed_register) << std::endl;
  }

  return newly_freed_register;
}

x86_Register Reg_allocator::allocate_reg(const Three_addr_var &var_to_be_allocated,
                                         int start_index, const CFG_node &node)
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

    /*  Only write to memory if register has a temporary variable that is used later in this block */
    if (reg_entry_var(reg_entry_being_freed).is_string() && dist_to_next_var_occurance(reg_entry_var(reg_entry_being_freed), start_index, node))
    {
      std::string source_reg_str = x86_Register_to_string(reg_entry_reg(reg_entry_being_freed));
      std::string var_offset_str = std::to_string(get_var_offset_cond_add(reg_entry_var(reg_entry_being_freed).to_string()));
      m_asm_file << "\tmov " << source_reg_str << ", -" << var_offset_str << "(%rbp)" << std::endl;
      m_debug_log << "storing variable " << reg_entry_var(reg_entry_being_freed).to_string() << " with offset " << var_offset_str
                  << std::endl;
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

void Reg_allocator::free(x86_Register reg_to_free, const CFG_node &node)
{

  int reg_index = static_cast<int>(reg_to_free);

  /*  Only free register if it hasn't been freed yet  */
  if (!m_register_free_status.at(reg_index))
  {
    m_free_reg_stack.push(reg_to_free);

    /* Write variable in register to memory if that variable is in the live out set of node */
    Three_addr_var &var_being_freed = reg_entry_var(m_allocated_reg_data.at(static_cast<int>(reg_to_free)));
    if (var_being_freed.is_string() && node.m_live_out.count(var_being_freed.to_string()) != 0)
    {
      std::string source_reg_str = x86_Register_to_string(reg_to_free);
      std::string var_offset_str = std::to_string(get_var_offset_cond_add(var_being_freed.to_string()));
      m_asm_file << "\tmov " << source_reg_str << ", -" << var_offset_str << "(%rbp)" << std::endl;
      m_debug_log << "storing variable " << var_being_freed.to_string() << " with offset " << var_offset_str
                  << std::endl;
    }

    reg_entry_var(m_allocated_reg_data.at(reg_index)) = Three_addr_var();
    reg_entry_dist(m_allocated_reg_data.at(reg_index)) = std::nullopt;
    m_register_free_status[reg_index] = true;
  }
}

std::optional<int> Reg_allocator::dist_to_next_var_occurance(const Three_addr_var &search_var,
                                                             int start_index, const CFG_node &node)
{
  for (int cur_index = start_index; cur_index <= node.m_end_index; cur_index++)
  {

    three_addr_code_entry &cur_entry = m_three_addr_code.at(cur_index);

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
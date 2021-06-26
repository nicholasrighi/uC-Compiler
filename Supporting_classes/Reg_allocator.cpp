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
  case x86_Register::RDX:
    return "%rdx";
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
  m_asm_file << "main:" << std::endl;
  m_asm_file << "\tmov %rsp, %rbp" << std::endl;
  m_asm_file << "\tadd $-8, %rbp" << std::endl;

  //for (int i = 0; i < static_cast<int>(x86_Register::RDI); i++)
  for (int i = 0; i < 4; i++)
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

    if (m_register_free_status[reg_to_index])
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

/*  
  Assumes all operations are of the form "x <- y op z", where y and z are optional
*/
void Reg_allocator::generate_varkill_uevar(CFG_node &node)
{
  for (int i = node.m_start_index; i < node.m_end_index; i++)
  {
    Three_addr_var x = std::get<0>(m_three_addr_code[node.m_start_index]);
    Three_addr_var y = std::get<2>(m_three_addr_code[node.m_start_index]);
    Three_addr_var z = std::get<3>(m_three_addr_code[node.m_start_index]);

    if (x.is_valid())
    {
      node.m_var_kill.insert(x.to_string());
    }

    if (y.is_valid() && y.is_string() && !node.m_var_kill.count(y.to_string()))
    {
      node.m_uevar.insert(y.to_string());
    }

    if (z.is_valid() && z.is_string() && !node.m_var_kill.count(z.to_string()))
    {
      node.m_uevar.insert(z.to_string());
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
    This is a stub that need to be fleshed out later. It assumes the entire program is one basic
    block. 
*/
void Reg_allocator::generate_CFG()
{
  m_cfg_graph.push_back(std::make_pair(CFG_node(0, m_three_addr_code.size()), false));
}

/*
  This is a stub that needs to be fleshed out later. Does nothing currently
*/
void Reg_allocator::create_live_out()

{
  /*
  bool changed = true;
  while (changed)
  {
    changed = false;
    std::set<std::string> saved_live_out = m_cfg_graph[0].first.m_live_out;
  }
  */
}

void Reg_allocator::gen_asm_line(x86_Register result_reg, Three_addr_OP op, x86_Register reg_1, x86_Register reg_2)
{
  std::string op1;
  std::string op2;
  std::string op1_debug;
  std::string op2_debug;

  auto write_to_file = [&](std::ofstream &file, std::string s1, std::string s2)
  {
    file << s1 << std::endl;
    file << s2 << std::endl;
  };

  auto get_var_from_reg = [this](x86_Register reg)
  {
    return reg_entry_var(m_allocated_reg_data[static_cast<int>(reg)]).to_string();
  };

  switch (op)
  {
  case Three_addr_OP::LOAD:
    m_asm_file << "\tmov " << x86_Register_to_string(reg_1) << ", " << x86_Register_to_string(result_reg)
               << std::endl;
    break;
  case Three_addr_OP::STORE:
    m_debug_log << "Storing value from " << x86_Register_to_string(reg_1) << " into "
                << x86_Register_to_string(result_reg) << std::endl;
    break;
  case Three_addr_OP::ADD:
    op1 = "mov " + x86_Register_to_string(reg_1) + ", " + x86_Register_to_string(result_reg);
    op2 = "add " + x86_Register_to_string(reg_2) + ", " + x86_Register_to_string(result_reg);
    op1_debug = "mov " + get_var_from_reg(reg_1) + " to " + x86_Register_to_string(result_reg);
    op2_debug = "add " + get_var_from_reg(reg_2) + " to " + x86_Register_to_string(result_reg);
    m_asm_file << "#" << std::endl;
    write_to_file(m_asm_file, "\n\t#" + op1_debug, "\t#" + op2_debug);
    write_to_file(m_asm_file, "\t" + op1, "\t" + op2);
    break;
  case Three_addr_OP::SUB:
    m_asm_file << "\tmov " << x86_Register_to_string(reg_1) << ", " << x86_Register_to_string(result_reg)
               << std::endl;
    m_asm_file << "\tsub " << x86_Register_to_string(reg_2) << ", " << x86_Register_to_string(result_reg)
               << std::endl;
    break;
  case Three_addr_OP::MULT:
    m_asm_file << "\tmov " << x86_Register_to_string(reg_1) << ", " << x86_Register_to_string(result_reg)
               << std::endl;
    m_asm_file << "\timul " << x86_Register_to_string(reg_2) << ", " << x86_Register_to_string(result_reg)
               << std::endl;
    break;
  case Three_addr_OP::DIVIDE:
    m_asm_file << "Diving values from " << x86_Register_to_string(reg_1) << " and "
               << x86_Register_to_string(reg_2) << " into "
               << x86_Register_to_string(result_reg) << std::endl;
    break;
  case Three_addr_OP::RET:
    op1 = "\tmov " + x86_Register_to_string(reg_1) + ", " + "%rax";
    op2 = "\tret";
    op1_debug = "returning " + get_var_from_reg(reg_1);
    m_asm_file << "#" << std::endl;
    write_to_file(m_asm_file, op1, op2);
    write_to_file(m_debug_log, op1_debug, "");
    break;
  case Three_addr_OP::ASSIGN:
    if (reg_1 != result_reg)
    {
      m_asm_file << "\tmov " << x86_Register_to_string(reg_1) << ", " << x86_Register_to_string(result_reg)
                 << std::endl;
    }
    break;
  default:
    m_asm_file << "Error, invalid op code passed to gen_asm_line " << std::endl;
    break;
  }
}

void Reg_allocator::generate_assembly_from_CFG_node(const CFG_node &node)
{
  for (int i = node.m_start_index; i < node.m_end_index; i++)
  {
    Three_addr_var var_result = std::get<0>(m_three_addr_code[i]);
    Three_addr_var var_1 = std::get<2>(m_three_addr_code[i]);
    Three_addr_var var_2 = std::get<3>(m_three_addr_code[i]);

    x86_Register reg_1;
    x86_Register reg_2;
    x86_Register reg_result;

    if (var_1.is_valid())
    {
      reg_1 = ensure(var_1, i + 1, node);
    }

    if (var_2.is_valid())
    {
      reg_2 = ensure(var_2, i + 1, node);
    }

    if (var_result.is_valid())
    {
      reg_result = allocate_reg(var_result, i + 1, node);
    }

    gen_asm_line(reg_result, std::get<1>(m_three_addr_code[i]), reg_1, reg_2);

    if (var_1.is_valid())
    {
      std::optional<int> next_var_1_use = dist_to_next_var_occurance(var_1, i + 1, node);
      reg_entry_dist(m_allocated_reg_data[static_cast<int>(reg_1)]) = next_var_1_use.has_value() ? next_var_1_use.value() : INT32_MAX;
      print_next_var_use(var_1, next_var_1_use);
    }

    if (var_2.is_valid())
    {
      std::optional<int> next_var_2_use = dist_to_next_var_occurance(var_2, i + 1, node);
      reg_entry_dist(m_allocated_reg_data[static_cast<int>(reg_2)]) = next_var_2_use.has_value() ? next_var_2_use.value() : INT32_MAX;
      print_next_var_use(var_1, next_var_2_use);
    }

    if (var_result.is_valid())
    {
      std::optional<int> next_var_result_use = dist_to_next_var_occurance(var_result, i + 1, node);
      reg_entry_dist(m_allocated_reg_data[static_cast<int>(reg_result)]) = next_var_result_use.has_value() ? next_var_result_use.value() : INT32_MAX;
      print_next_var_use(var_result, next_var_result_use);
    }
    print_register_contents();
  }
}

x86_Register Reg_allocator::ensure(const Three_addr_var &var_to_be_allocated, int start_index, const CFG_node &node)
{

  /*  If a register already contains the value we're searching for, return that register */
  for (register_entry &entry : m_allocated_reg_data)
  {
    if (reg_entry_var(entry) == var_to_be_allocated)
    {
      return reg_entry_reg(entry);
    }
  }

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
    reg_entry_dist(m_allocated_reg_data.at(next_furthest_use_index)) = std::nullopt;
    m_register_free_status.at(next_furthest_use_index) = false;

    return static_cast<x86_Register>(next_furthest_use_index);
  }
}

void Reg_allocator::free(register_entry &reg_entry)
{

  int reg_index = static_cast<int>(reg_entry_reg(reg_entry));

  /*  Only free register if it hasn't been freed yet  */
  if (!m_register_free_status[reg_index])
  {
    m_free_reg_stack.push(reg_entry_reg(reg_entry));

    /*  
        Reset reg_entry to empty. Ensures that an entry won't compare to true 
        when iterating through the vector for matching values 
    */
    reg_entry_var(reg_entry) = Three_addr_var();
    reg_entry_dist(reg_entry) = std::nullopt;
    m_register_free_status[reg_index] = true;
  }
}

std::optional<int> Reg_allocator::dist_to_next_var_occurance(const Three_addr_var &search_var,
                                                             int start_index, const CFG_node &node)
{
  for (int cur_index = start_index; cur_index < node.m_end_index; cur_index++)
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
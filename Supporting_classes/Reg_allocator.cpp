#include "Reg_allocator.h"

Reg_allocator::Reg_allocator(std::string file_name, Program_symbol_table &sym_table, std::vector<three_addr_code_entry> &three_addr_code) : m_prog_sym_table(sym_table), m_three_addr_code(three_addr_code)
{
  m_asm_file.open(file_name, std::ofstream::trunc);

  /*  Allows registers r8 through r15 to be allocated */
  for (int i = 8; i < 16; i++) {
    m_free_physical_registers.push("r" + std::to_string(i));
  }
}

Reg_allocator::~Reg_allocator()
{
  m_asm_file.close();
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

void Reg_allocator::generate_assembly(CFG_node node)
{
  for (int i = node.m_start_index; i < node.m_end_index; i++)
  {
  }
}

std::string Reg_allocator::ensure(std::string var_name) {

  for (register_entry& entry : m_allocated_reg_data) {
    /*  If var is already in a register, return the allocated register */
    if ((std::get<0>(entry) == var_name) && std::get<3>(entry)) {
      return std::get<1>(entry);
    }
  }

  std::string allocated_register = allocate(var_name);

  

}

std::string Reg_allocator::allocate(std::string var_name) {

}
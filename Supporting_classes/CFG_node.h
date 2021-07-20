#pragma once

#include <optional>
#include <set>
#include <string>

class CFG_node
{

public:
  CFG_node(int start_index, int end_index);
  CFG_node(int start_index, int end_index, int jmp_target);
  CFG_node(int start_index, int end_index, int jmp_target, int fall_through_target);

  /*  The first instruction in the basic block */
  int m_start_index;

  /*  The instruction at the end of the basic block (inclusive) */
  int m_end_index;

  /*  The instruction that will be taken if a jump occurs */
  std::optional<int> m_jmp_target; 
  
  /*  The instruction that will be taken if a jump doesn't occur */
  std::optional<int> m_fall_through_target; 

  std::set<std::string> m_live_out;
  std::set<std::string> m_var_kill;
  std::set<std::string> m_uevar;

  bool operator==(const CFG_node& other);
  bool operator!=(const CFG_node& other);
};
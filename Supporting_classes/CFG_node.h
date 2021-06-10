#pragma once

#include <optional>
#include <set>
#include <string>

class CFG_node
{

public:
  CFG_node(int start_index, int end_index);
  CFG_node(int start_index, int end_index, int child_one_index);
  CFG_node(int start_index, int end_index, int child_one_index, int child_two_index);

  /*  The first instruction in the basic block */
  int m_start_index;
  /*  The instruction immediately after the end of the basic block */
  int m_end_index;
  std::optional<int> m_child_one_index = std::nullopt;
  std::optional<int> m_child_two_index = std::nullopt;
  std::set<std::string> m_live_out;
  std::set<std::string> m_var_kill;
  std::set<std::string> m_uevar;
};
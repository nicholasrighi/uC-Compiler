/*  Implements */
#include "CFG_node.h"

CFG_node::CFG_node(int start_index, int end_index) : m_start_index(start_index), m_end_index(end_index)
{
}

CFG_node::CFG_node(int start_index, int end_index, int child_one_index) : m_start_index(start_index), m_end_index(end_index), m_child_one_index(child_one_index)
{
}

CFG_node::CFG_node(int start_index, int end_index, int child_one_index, int child_two_index) : m_start_index(start_index), m_end_index(end_index), m_child_one_index(child_one_index), m_child_two_index(child_two_index)
{
}
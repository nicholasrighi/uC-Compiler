/*  Implements */
#include "CFG_node.h"

CFG_node::CFG_node(int start_index, int end_index) : m_start_index(start_index), m_end_index(end_index)
{
}

CFG_node::CFG_node(int start_index, int end_index, int jmp_target)
    : m_start_index(start_index), m_end_index(end_index), m_jmp_target(jmp_target)
{
}

CFG_node::CFG_node(int start_index, int end_index, int jmp_target, int fall_through_target)
    : m_start_index(start_index), m_end_index(end_index), m_jmp_target(jmp_target), m_fall_through_target(fall_through_target)
{
}
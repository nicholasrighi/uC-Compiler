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

bool CFG_node::operator==(const CFG_node &other)
{
    return (m_start_index == other.m_start_index &&
            m_end_index == other.m_end_index &&
            m_jmp_target == other.m_jmp_target &&
            m_fall_through_target == other.m_fall_through_target &&
            m_live_out == other.m_live_out &&
            m_var_kill == other.m_var_kill &&
            m_uevar == other.m_uevar);
}

bool CFG_node::operator!=(const CFG_node &other)
{
    return !(*this == other);
}
#include "Var_dec.h"

Var_dec::Var_dec(Var_ref *name, Ret_type var_type, Object_type obj_type)
    : m_var(name), m_var_type(var_type), m_obj_type(obj_type) {}

void Var_dec::accept(Abstract_visitor &visitor)
{
  visitor.dispatch(*this);
}

std::string Var_dec::get_name()
{
  return m_var->m_name;
}

std::string Var_dec::get_type_string()
{
  return type_to_string(m_var_type);
}
#pragma once

#include "Base_node.h"
#include "Var_ref.h"

class Var_dec : public Base_node
{
public:
  Var_dec(Var_ref *name, Ret_type var_type, Object_type obj_type) : m_var(name), m_var_type(var_type), m_obj_type(obj_type) {}

  void accept(Abstract_visitor &visitor) override
  {
    visitor.dispatch(*this);
  }

  std::string get_name() { return m_var->m_name; }

  std::string get_type_string()
  {
    switch (m_var_type)
    {
    case Ret_type::INT:
      return "INT";
    case Ret_type::CHAR:
      return "CHAR";
    case Ret_type::VOID:
      return "VOID";
    default:
      return "ERROR";
    }
  }

  /* holds var name*/
  Var_ref *m_var;
  
  /* Variable type that user declares (int, char) */
  Ret_type m_var_type;

  /* what type is stored in the symbol table (scalar, array, func) */
  Object_type m_obj_type;
};
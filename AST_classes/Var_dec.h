#pragma once

#include "Base_node.h"
#include "Var_ref.h"

class Var_dec : public Base_node
{
public:
  Var_dec(Var_ref *name, Ret_type var_type, Object_type obj_type);

  void accept(Abstract_visitor &visitor) override;

  std::string get_name();

  std::string get_type_string();

  /* holds var name*/
  Var_ref *m_var;

  /* Variable type that user declares (int, char) */
  Ret_type m_var_type;

  /* what type is stored in the symbol table (scalar, array, func) */
  Object_type m_obj_type;
};
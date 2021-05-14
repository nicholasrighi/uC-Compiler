#pragma once

#include "Base_node.h"
#include "Var_ref.h"

class Var_dec : public Base_node
{
public:
  Var_dec(Var_ref* name, Var_type var_type) : m_var(name), m_var_type(var_type) {}

  void accept(Abstract_visitor& visitor) override {
    visitor.dispatch(*this);
  }

  std::string get_name() {return m_var->m_name;}

  std::string get_type_string() {
    switch(m_var_type) {
      case Var_type::INT:
        return "INT";
      case Var_type::CHAR:
        return "CHAR";
      case Var_type::VOID:
        return "VOID";
      default:
        return "ERROR";
    }
  }

  Var_ref* m_var;
  Var_type m_var_type;
};
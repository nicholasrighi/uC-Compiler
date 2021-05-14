#pragma once

#include <string>
#include "Base_node.h"

// Var_type specifies the types that can be returned from a function or used as variable types
// note: void cannot be a variable type in uC, so symantic analysis needs to ensure that
// no variables of type void are declared
enum class Var_type
{
  INT,
  CHAR,
  VOID
};

// Ref_type specifies the type of reference that is being used. 
enum class Ref_type
{
  VAR,
  ARRAY,
  FUNC,
  ERROR
};

class Var_ref : public Base_node
{
public:
  // Construct variable reference for a non array variable. m_ref_type can't be known when a ref_type object
  // is returned by the lexer, so m_ref_type is set to ERROR
  Var_ref(char *name) : m_name(name), m_ref_type(Ref_type::ERROR), m_access_index(-1) {}

  void accept(Abstract_visitor& visitor) override {
    visitor.dispatch(*this);
  }
  std::string m_name;
  
  // the ref type needs to be set by the parser once the type is known. The rule is of the form "typename ID", 
  // so typename allows m_ref_type to be set 
  Ref_type m_ref_type;
  int m_access_index;
};
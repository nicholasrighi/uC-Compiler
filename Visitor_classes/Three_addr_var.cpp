#include "Three_addr_var.h"

/*  Used to indicate that the Three_addr_var isn't used and should be ignored*/
Three_addr_var::Three_addr_var() : m_constant(std::nullopt), m_temp_var(std::nullopt),
                                   m_var_type(Three_addr_var_type::EMPTY) {}

/*  Used to indicate that the Three_addr_var is holding a constant */
Three_addr_var::Three_addr_var(int val) : m_constant(val), m_temp_var(std::nullopt),
                                          m_var_type(Three_addr_var_type::CONSTANT) {}

/*  Used to indicate that the Three_addr_var is holding the name of a scalar variable, either temporary or named */
Three_addr_var::Three_addr_var(std::string temp_var, Three_addr_var_type var_type)
    : m_constant(std::nullopt), m_temp_var(temp_var), m_var_type(var_type) {}

/*  
      Returns the string representation of the stored variable or constant. If neither a 
      variable nor a string constant are defined, it returns an error message
  */
std::string Three_addr_var::to_string() const
{
  if (m_var_type == Three_addr_var_type::CONSTANT)
  {
    return std::to_string(*m_constant);
  }
  else if (m_var_type != Three_addr_var_type::EMPTY)
  {
    return *m_temp_var;
  }
  else
  {
    return "ERROR IN 3 ADDR VAR ACCESS";
  }
}

/* determine if this object has valid state, which means it either holds a constant or a temporary variable name */
bool Three_addr_var::is_valid() const
{
  return m_var_type != Three_addr_var_type::EMPTY;
}

bool Three_addr_var::is_string() const
{
  return m_var_type == Three_addr_var_type::SCALAR_VAR;
}

bool Three_addr_var::is_raw_str() const {
  return m_var_type == Three_addr_var_type::RAW_STR;
}

bool Three_addr_var::is_label() const {
  return m_var_type == Three_addr_var_type::LABEL;
}

bool Three_addr_var::is_const() const {
  return m_var_type == Three_addr_var_type::CONSTANT;
}

bool Three_addr_var::is_array() const {
  return m_var_type == Three_addr_var_type::ARRAY;
}

bool Three_addr_var::operator==(const Three_addr_var &other) const
{
  if (this->is_valid() != other.is_valid())
  {
    return false;
  }
  if (this->is_string())
  {
    return other.is_string() && (m_temp_var == other.m_temp_var);
  }
  else
  {
    return !other.is_string() && (m_constant == other.m_constant);
  }
}

bool Three_addr_var::operator!=(const Three_addr_var &other) const
{
  return !(*this == other);
}
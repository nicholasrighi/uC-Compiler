#include "Three_addr_var.h"

/*  Used to indicate that the Three_addr_var isn't used and should be ignored*/
Three_addr_var::Three_addr_var() : m_constant(std::nullopt), m_temp_var(std::nullopt) {}

/*  Used to indicate that the Three_addr_var is holding a constant */
Three_addr_var::Three_addr_var(int val) : m_constant(val), m_temp_var(std::nullopt) {}

/*  Used to indicate that the Three_addr_var is holding the name of a scalar variable, either temporary or named */
Three_addr_var::Three_addr_var(std::string temp_var, bool is_array)
    : m_constant(std::nullopt), m_temp_var(temp_var), m_is_array(is_array) {}

/*  
      Returns the string representation of the stored variable or constant. If neither a 
      variable nor a string constant are defined, it returns an error message
  */
std::string Three_addr_var::to_string() const
{
  /*  constant reference */
  if (m_constant)
  {
    return std::to_string(*m_constant);
  }
  /*  constant reference */
  else if (m_temp_var)
  {
    return *m_temp_var;
  }
  /*  
      this being returned signals an error condition. is_valid() should always be called before
      accessing the value of a Three_addr_var, so this part of the if-else block executing is the result of an error 
    */
  else
  {
    return "ERROR IN 3 ADDR VAR ACCESS";
  }
}

/* determine if this object has valid state, which means it either holds a constant or a temporary variable name */
bool Three_addr_var::is_valid() const
{
  return m_constant.has_value() || m_temp_var.has_value();
}

bool Three_addr_var::is_string() const
{
  return m_temp_var.has_value();
}

bool Three_addr_var::operator==(const Three_addr_var &other) const
{
  if (this->is_valid() != other.is_valid()) 
  {
    return false;
  }
  if (this->is_string()) {
    return other.is_string() && (m_temp_var == other.m_temp_var);
  } else {
    return !other.is_string() && (m_constant == other.m_constant);
  }
}

bool Three_addr_var::operator!=(const Three_addr_var &other) const
{
  return !(*this == other);
}
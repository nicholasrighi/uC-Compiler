#pragma once

#include <string>
#include <optional>

class Three_addr_var
{
public:
  /*  Used to indicate that the Three_addr_var isn't used and should be ignored*/
  Three_addr_var() : m_constant(std::nullopt), m_temp_var(std::nullopt) {}

  /*  Used to indicate that the Three_addr_var is holding a constant */
  Three_addr_var(int val) : m_constant(val), m_temp_var(std::nullopt) {}

  /*  Used to indicate that the Three_addr_var is holding the name of a scalar variable, either temporary or named */
  Three_addr_var(std::string temp_var) : m_constant(std::nullopt), m_temp_var(temp_var) {}

  /*  Returns the string representation of the stored variable or constant. If neither a variable nor a string constant are defined, it returns ERROR */
  std::string to_string()
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
  bool is_valid()
  {
    return m_constant.has_value() || m_temp_var.has_value();
  }

  bool is_string() {
    return m_temp_var.has_value();
  }

  std::optional<int> m_constant;
  std::optional<std::string> m_temp_var;
};
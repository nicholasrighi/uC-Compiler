#pragma once

#include <string>
#include <optional>

class Three_addr_var
{
public:
  /*  Used to indicate that the Three_addr_var isn't used and should be ignored*/
  Three_addr_var();

  /*  Used to indicate that the Three_addr_var is holding a constant */
  Three_addr_var(int val);

  /*  Used to indicate that the Three_addr_var is holding the name of a scalar variable, either temporary or named */
  Three_addr_var(std::string temp_var);

  /*  
      Returns the string representation of the stored variable or constant. If neither a 
      variable nor a string constant are defined, it returns an error message
  */
  std::string to_string() const;

  /* determine if this object has valid state, which means it either holds a constant or a temporary variable name */
  bool is_valid() const;

  /*  returns true if the Three_addr_var holds a string, flase otherwise */
  bool is_string() const;

  /*  
      This returns true only if both Three_addr_var objects contain the same
      data or both are empty
  */
  bool operator==(const Three_addr_var &other) const;
  bool operator!=(const Three_addr_var &other) const;

  std::optional<int> m_constant;
  std::optional<std::string> m_temp_var;
};

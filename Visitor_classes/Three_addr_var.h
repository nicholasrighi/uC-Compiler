#pragma once

#include <string>
#include <optional>

enum class Three_addr_var_type
{
  CONSTANT,     //  a numerical constant 
  SCALAR_VAR,   //  a named variable (user defined or compiler generated) that holds a single value
  ARRAY,        //  a named variable that is an array
  LABEL,        //  the target of a jump instruction
  RAW_STR,      //  indicates that the string contained in the three_addr_var should be written 
                //      directly to the assembly file 
  EMPTY         //  indicates that the three_addr_code holds no information
};

class Three_addr_var
{
public:
  /*  Used to indicate that the Three_addr_var isn't used and should be ignored*/
  Three_addr_var();

  /*  Used to indicate that the Three_addr_var is holding a constant */
  Three_addr_var(int val);

  /*  
    Used to indicate that the Three_addr_var is holding the name of a scalar variable, 
    either temporary or named 
  */
  Three_addr_var(std::string temp_var, Three_addr_var_type var_type = Three_addr_var_type::SCALAR_VAR);

  /*  
      Returns the string representation of the stored variable or constant. If neither a 
      variable nor a string constant are defined, it returns an error message
  */
  std::string to_string() const;

  /* determine if this object has valid state, which means it either holds a constant or a temporary variable name */
  bool is_valid() const;

  /*  returns true if the Three_addr_var holds a scalar variable, false otherwise */
  bool is_string() const;

  /*  returns true if the Three_addr_var contains an array, false otherwise */
  bool is_array() const;

  /* returns true if the Three_addr_var contains a raw string, false otherwise */
  bool is_raw_str() const;

  /* returns true if the Three_addr_var contains a label, false otherwise */
  bool is_label() const;

  /* returns true if the Three_addr_var contains a numerical constant, false otherwise */
  bool is_const() const;

  /*  
      This returns true only if both Three_addr_var objects contain the same
      data or both are empty
  */
  bool operator==(const Three_addr_var &other) const;
  bool operator!=(const Three_addr_var &other) const;

private:
  std::optional<int> m_constant;
  std::optional<std::string> m_temp_var;
  Three_addr_var_type m_var_type;
};

#pragma once

#include <string>

/*  
   Var_type specifies the types that can be returned from a function or used as variable types
   note: void cannot be a variable type in uC, so symantic analysis needs to ensure that
   no variables of type void are declared
*/
enum class Ret_type
{
  INT,
  CHAR,
  VOID
};

/* Ref_type specifies the type of reference that is being used */
enum class Object_type
{
  SCALAR,
  ARRAY,
  FUNC,
  ERROR
};

/*  Where the variable will be located in memory */
enum class Var_storage{
  LOCAL,
  GLOBAL,
  REGISTER
};

/*  Returns a string representation of the specified Ret_type */
std::string type_to_string(Ret_type type);
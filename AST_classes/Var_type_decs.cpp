#include "Var_type_decs.h"

std::string type_to_string(Ret_type type)
{
  switch (type)
  {
  case Ret_type::INT:
    return "INT";
  case Ret_type::CHAR:
    return "CHAR";
  case Ret_type::VOID:
    return "VOID";
  default:
    return "ERROR, INVALID TYPE";
  }
}
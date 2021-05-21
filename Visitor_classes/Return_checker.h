#pragma once

// Inherits from
#include "Abstract_visitor.h"

/*
  this class checks that all non-void functions always return a value through all paths of control flow
*/
class Return_checker : public Abstract_visitor
{
public:
  void dispatch(Array_access &node);
  void dispatch(Array_dec &node);
  void dispatch(Binop_dec &node);
  void dispatch(Func_dec &node);
  void dispatch(Func_ref &node);
  void dispatch(If_dec &node);
  void dispatch(Number &node);
  void dispatch(Return_dec &node);
  void dispatch(Stmt_dec &node);
  void dispatch(Unop_dec &node);
  void dispatch(Var_dec &node);
  void dispatch(Var_ref &node);
  void dispatch(While_dec &node);

  /* 
     returns if the parser has found a valid return statement for every path of control flow
     in the most recent function it parsed
  */
  bool parse_status()
  {
    return m_found_return_for_all_paths;
  }

private:
  /* flag for determining if the current recusive path has a return statement */
  bool m_returns;
  /* 
    flag for determining if the current function has a return statement for all
    paths of control flow
  */
  bool m_found_return_for_all_paths;
};
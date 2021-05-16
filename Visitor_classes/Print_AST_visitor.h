#pragma once

// inherits from
#include "Abstract_visitor.h"

// system includes
#include <iostream>

class Print_AST_visitor : public Abstract_visitor
{
public:
  void dispatch(Array_access &node) override;
  void dispatch(Array_dec& node) override;
  void dispatch(Binop_dec& node) override;
  void dispatch(Func_dec& node) override;
  void dispatch(Func_ref& node) override;
  void dispatch(If_dec& node) override;
  void dispatch(Number& node) override;
  void dispatch(Return_dec& node) override;
  void dispatch(Stmt_dec& node) override;
  void dispatch(Unop_dec& node) override;
  void dispatch(Var_dec& node) override;
  void dispatch(Var_ref& node) override;
  void dispatch(While_dec& node) override;
};

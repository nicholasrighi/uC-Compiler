#pragma once

class Array_access;
class Array_dec;
class Array_ref;
class Binop_dec;
class Func_dec;
class Func_ref;
class If_dec;
class Number;
class Return_dec;
class Stmt_dec;
class Unop_dec;
class Var_dec;
class Var_ref;
class While_dec;

class Abstract_visitor {
  public:
    virtual void dispatch(Array_access& node) = 0;
    virtual void dispatch(Array_dec& node) = 0;
    virtual void dispatch(Binop_dec& node) = 0;
    virtual void dispatch(Func_dec& node) = 0;
    virtual void dispatch(Func_ref& node) = 0;
    virtual void dispatch(If_dec& node) = 0;
    virtual void dispatch(Number& node) = 0;
    virtual void dispatch(Return_dec& node) = 0;
    virtual void dispatch(Stmt_dec& node) = 0;
    virtual void dispatch(Unop_dec& node) = 0;
    virtual void dispatch(Var_dec& node) = 0;
    virtual void dispatch(Var_ref& node) = 0;
    virtual void dispatch(While_dec& node) = 0;
};
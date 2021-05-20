/* 
    Code requires goes near the top of the *.tab.h file, above the %union decleration. The AST header
    files define the types the union uses, so we need these header files to appear before the 
    %union decleration. Also makes the headers avaliable to the flex scanner.
*/
%code requires {
  // AST classes
  #include "AST_classes/Number.h"
  #include "AST_classes/Var_dec.h"
  #include "AST_classes/Array_dec.h"
  #include "AST_classes/Array_access.h"
  #include "AST_classes/Func_dec.h"
  #include "AST_classes/Func_ref.h"
  #include "AST_classes/Return_dec.h"
  #include "AST_classes/While_dec.h"
  #include "AST_classes/If_dec.h"
  #include "AST_classes/Stmt_dec.h"
  #include "AST_classes/Var_ref.h"
  #include "AST_classes/Binop_dec.h"
  #include "AST_classes/Unop_dec.h"
}

%union {
    Base_node* abstract_ptr;
    Ret_type    variable_type;  // used when a variable type is being defined (only used for the typename token)
    Var_ref*    variable_ref;   // used for naming a variable (if you need to represent x as an input to f(x))
    Var_dec*    variable_dec;   // used when defining an int or a char (int x;)
    Array_dec*  array_dec;      // used when defining an array (int x[3];)
    Func_dec*   function_dec;   // used when defining a function (void x())
    Stmt_dec*   stmt_dec;       // used when defining a list of exp* (or some subclass of exp*)
    Binop_dec*  binop_dec;      // used for defining a binary operation (*,+,-, etc)
    Unop_dec*   unop_dec;       // used for defining a uniary operation (!, -)
    int         value;          // used when defining a constant (return 3 * 4)
}

/*
    The code decleration only appears in the *.tab.c file, so we define parser implementation only details here
*/
%code {

  /* system references */
  #include <stdio.h>
  #include <stdlib.h>

  /* needed by yyparse */
  extern "C" int yylex();

  /* the line number currently being processed */ 
  extern int line_number;

  /* needed by bison to report errors */
  void yyerror(char* s);

  /* the root of the AST */
  Stmt_dec* root;
}

%type <abstract_ptr>  expr condition
%type <stmt_dec>      expr_list actuals stmt stmts else_part program 
                      topdec_list formals funbody locals formal_list
%type <variable_dec>  topdec vardec scalardec formaldec
%type <array_dec>     arraydec
%type <variable_type> typename
%type <unop_dec>      unop 
%type <binop_dec>     binop

// control flow
%token ELSE   "else"
%token IF     "if"
%token WHILE  "while"
%token RET    "return"

// type declerations
%token TYPE_INT   "int"
%token TYPE_CHAR  "char"
%token TYPE_VOID  "void"

// operators
%token <expression>  OP_ASSIGN          "=" 
%token <expression>  OP_LOG_AND         "&&"
%token <expression>  OP_NOT_EQUAL       "!="
%token <expression>  OP_EQUAL           "=="
%token <expression>  OP_LESS_EQUAL      "<="
%token <expression>  OP_LESS_THAN       "<"
%token <expression>  OP_GREATER_EQUAL   ">="
%token <expression>  OP_GREATER_THAN    ">"
%token <expression>  OP_MINUS           "-"
%token <expression>  OP_ADD             "+"
%token <expression>  OP_MULT            "*"
%token <expression>  OP_DIVIDE          "/"
%token <expression>  UMINUS 
%token <expression>  OP_INVERT          "!"

// TODO. FIX UMINUS
%right "="
%left "&&"
%left "==" "!="
%left "<" ">" "<=" ">="
%left "+" "-"
%left "*" "/"
%left "!"

// misc symbols
%token LEFT_PAREN     "("
%token RIGHT_PAREN    ")"
%token LEFT_BRACKET   "["
%token RIGHT_BRACKET  "]"
%token LEFT_CURLY     "{"
%token RIGHT_CURLY    "}"
%token SEMICOLON      ";"
%token COMMA          ","
%token <variable_ref> ID
%token <value> NUMBER

%%

program         : topdec_list {root = $1;}

topdec_list     : topdec topdec_list  {
                                      // this assignment format means that we avoid creating a 
                                      // tree of nodes and instead end up with single node with
                                      // n direct children. See http://web.eecs.utk.edu/~bvanderz/teaching/cs461Sp11/notes/parse_tree/
                                      // for details
                                       $$ = $2;
                                       $$->add_expression_front($1);
                                      }

                | %empty              {$$ = new Stmt_dec;}
                ;

topdec          : vardec ";"          {$$ = $1;}

                | typename ID "(" formals ")" funbody {$$ = new Func_dec($2, $1, $4, $6);}
                ;

vardec          : scalardec {$$ = $1;}
                | arraydec  {$$ = $1;}
                ;

scalardec       : typename ID {$$ = new Var_dec($2, $1, Object_type::SCALAR);}
                ;

arraydec        : typename ID "[" NUMBER "]" {$$ = new Array_dec($2, $1, $4);}
                ;

typename        : "int"     {$$ = Ret_type::INT;}   
                | "char"    {$$ = Ret_type::CHAR;}
                | "void"    {$$ = Ret_type::VOID;}
                ;

funbody         : "{" locals stmts "}" {
                                        $$ = new Stmt_dec; 

                                        for (auto& sub_exp : $2->m_sub_expressions) {
                                          $$->add_expression_back(sub_exp);
                                        }

                                        for (auto& sub_exp : $3->m_sub_expressions) {
                                          $$->add_expression_back(sub_exp);
                                        }

                                        free($2);
                                        free($3);
                                       }

                | ";"                  {$$ = nullptr;}
                ;

formals         : "void"      {$$ = nullptr;}
                | formal_list {$$ = $1;}
                ;

formal_list     : formaldec                  {
                                              $$ = new Stmt_dec;
                                              $$->add_expression_back($1);
                                             }

                | formaldec "," formal_list  {
                                              $$ = $3;
                                              $$->add_expression_front($1);
                                             }
                ;

formaldec       : scalardec                  {$$ = $1;}

                | typename ID "[" "]"        {$$ = new Array_dec($2, $1, -1);}
                ;

locals          : vardec ";" locals           {
                                               $$ = $3;
                                               $$->add_expression_front($1);
                                              }
                | %empty                      {$$ = new Stmt_dec;}
                ;

stmts           : stmt stmts                    {
                                                 $$ = $2;
                                                 $$->add_expression_front($1);
                                                }

                | %empty                        {$$ = new Stmt_dec;}
                ;

stmt            : expr ";"                      {
                                                  $$ = new Stmt_dec;
                                                  $$->add_expression_back($1);
                                                }

                | "return" expr ";"             {
                                                 $$ = new Stmt_dec;
                                                 $$->add_expression_back(new Return_dec($2));
                                                }

                | "return" ";"                  {
                                                 $$ = new Stmt_dec;
                                                 $$->add_expression_back(new Return_dec(nullptr));
                                                }

                | "while" condition stmt        {
                                                 $$ = new Stmt_dec;
                                                 $$->add_expression_back(new While_dec($2, $3));
                                                }

                | "if" condition stmt else_part {
                                                 $$ = new Stmt_dec;
                                                 $$->add_expression_back(new If_dec($2, $3, $4));
                                                }

                | "{" stmts "}"                 {
                                                 $$ = $2;
                                                 //TODO. decide if this is necessary. Can't declare variables inside a stmt, do 
                                                 // this doesn't really do anything 
                                                 $$->m_new_scope = true;
                                                }

                | ";"                           {$$ = nullptr;}
                ;

else_part       : "else" stmt {$$ = $2;}
                | %empty      {$$ = nullptr;}
                ;

condition       : "(" expr ")" {$$ = $2;}
                ;

expr            : NUMBER  {$$ = new Number($1);}

                | ID      {$$ = $1;}

                | ID "[" expr "]" {$$ = new Array_access($1, $3);}

                | unop expr       {
                                   $1->add_expression($2);
                                   $$ = $1;
                                   }

                | expr binop expr     {
                                        $2->add_left_exp($1);
                                        $2->add_right_exp($3);
                                        $$ = $2;
                                      }

                | ID "(" actuals ")" {$$ = new Func_ref($1, $3);}

                | "(" expr ")"        {$$ = $2;}
                ;

unop            : "-"   {$$ = yylval.unop_dec;}
                | "!"   {$$ = yylval.unop_dec;}
                ;

binop           : "+"   {$$ = yylval.binop_dec;}
                | "-"   {$$ = yylval.binop_dec;}
                | "*"   {$$ = yylval.binop_dec;}
                | "/"   {$$ = yylval.binop_dec;}
                | "<"   {$$ = yylval.binop_dec;}
                | ">"   {$$ = yylval.binop_dec;}
                | "<="  {$$ = yylval.binop_dec;}
                | ">="  {$$ = yylval.binop_dec;}
                | "!="  {$$ = yylval.binop_dec;}
                | "=="  {$$ = yylval.binop_dec;}
                | "&&"  {$$ = yylval.binop_dec;}
                | "="   {$$ = yylval.binop_dec;}
                ;

actuals         : expr_list {$$ = $1;}
                | %empty    {$$ = nullptr;}
                ;

expr_list       : expr                 {  
                                        $$ = new Stmt_dec;
                                        $$->add_expression_back($1);
                                       }

                | expr "," expr_list   {
                                        $$ = $3;
                                        $$->add_expression_front($1);
                                       }
                ;

%%

void yyerror(char* s) {
  printf("Error on line %d, %s\n", line_number, s);
  exit(1);
}
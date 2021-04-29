%{
  #include <stdio.h>
  #include <stdlib.h>
  #define YYDEBUG 1
  int yylex();
  void yyerror(char* s) {
    exit(1);
  }
%}

// variables types
%token TYPE_INT    "int"
%token TYPE_CHAR   "char"
%token TYPE_VOID   "void"

// control flow
%token ELSE   "else"
%token IF     "if"
%token WHILE  "while"
%token RET    "return"

// operators
%token OP_MINUS           "-"
%left OP_MULT            "*"
%left OP_DIVIDE          "/"
%token OP_ADD             "+"
%token OP_NOT_EQUAL       "!="
%token OP_LOG_AND         "&&"
%token OP_LESS_EQUAL      "<="
%token OP_LESS_THAN       "<"
%token OP_EQUAL           "=="
%token OP_ASSIGN          "=" 
%token OP_GREATER_EQUAL   ">="
%token OP_GREATER_THAN    ">"

%token OP_INVERT          "!"

// misc symbols
%token LEFT_PAREN     "("
%token RIGHT_PAREN    ")"
%token LEFT_BRACKET   "["
%token RIGHT_BRACKET  "]"
%token LEFT_CURLY     "{"
%token RIGHT_CURLY    "}"
%token SEMICOLON      ";"
%token COMMA          ","
%token ID
%token NUMBER

%%

program         : topdec_list

topdec_list     : topdec topdec_list
                | %empty
                ;

topdec          : vardec ";"
                | typename ID "(" formals ")" funbody
                ;

vardec          : scalardec 
                | arraydec
                ;

scalardec       : typename ID
                ;

arraydec        : typename ID "[" NUMBER "]"
                ;

typename        : TYPE_INT
                | TYPE_CHAR
                | TYPE_VOID
                ;

funbody         : "{" locals stmts "}" 
                | ";"
                ;

formals         : "void" 
                | formal_list
                | %empty
                ;

formal_list     : formaldec 
                | formaldec "," formal_list
                ;

formaldec       : scalardec 
                | typename ID "[" "]"
                ;

locals          : vardec ";" locals
                | %empty
                ;

stmts           : stmt stmts
                | %empty
                ;

stmt            : expr ";"
                  | "return" expr ";" 
                  | "return" ";"
                  | "while" condition stmt
                  | "if" condition stmt else_part
                  | "{" stmts "}"
                  | ";"
                  ;

else_part       : "else" stmt
                | %empty
                ;

condition       : "(" expr ")"
                ;

expr            : NUMBER 
                  | ID 
                  | ID "[" expr "]"
                  | unop expr
                  | expr binop expr
                  | ID "(" actuals ")"
                  | "(" expr ")"
                  ;

unop            : "-" 
                | "!"

binop           : "+" | "-" | "*" | "/"
                  | "<" | ">" | "<=" | ">=" | "!=" | "=="
                  | "&&"
                  | "="
                  ;

actuals         : expr_list
                | %empty
                ;

expr_list       : expr 
                | expr "," expr_list
                ;

%%

#include "lex.yy.c"

int main(int argc, char** argv) {
    if (argc < 2) {
      printf("error, please supply a file to compile\n");
      return -1;
    }

     yyin = fopen(argv[1], "r");
     if (!yyin) {
       printf("error, invalid file name\n");
       return -1;
     }
    yyparse();
}
%{
    /* needed to make sure g++ doesn't managle yylex() */
    extern "C" int yylex();
    #include "grammar.tab.h"

    int line_number = 1;
%}

/* regular definitions */
delim   [ \t]
letter  [A-Za-z_]
digit   [0-9]
number  [0-9]+
id      {letter}({letter}|{digit})*

%%

    /* whitespace */
{delim}   {/*do nothing*/}
\n        {line_number++;}

    /* variable types */
int       {return TYPE_INT;}
char      {return TYPE_CHAR;}
void      {return TYPE_VOID;}

    /* control flow */
else      {return ELSE;}
if        {return IF;}
while     {return WHILE;}
return    {return RET;}

    /* operators */
"!"       {yylval.unop_dec = new Unop_dec(yytext); return OP_INVERT;}
"!="      {return OP_NOT_EQUAL;}
"&&"      {return OP_LOG_AND;}
"&"       {return OP_BITWISE_AND;}
"|"       {return OP_BITWISE_OR;}
"||"      {return OP_LOG_OR;}
"*"       {return OP_MULT;}
"+"       {return OP_ADD;}
"-"       {return OP_MINUS;}
"/"       {return OP_DIVIDE;}
"<="      {return OP_LESS_EQUAL;}
"<"       {return OP_LESS_THAN;}
"=="      {return OP_EQUAL;}
"="       {return OP_ASSIGN;}
">="      {return OP_GREATER_EQUAL;}
">"       {return OP_GREATER_THAN;}

    /* misc symbols */
"("       {return LEFT_PAREN;}
")"       {return RIGHT_PAREN;}
"["       {return LEFT_BRACKET;}
"]"       {return RIGHT_BRACKET;}
"{"       {return LEFT_CURLY;}
"}"       {return RIGHT_CURLY;}
";"       {return SEMICOLON;}
","       {return COMMA;}
{id}      {yylval.variable_ref = new Var_ref(yytext); return ID;}
{number}  {yylval.value = atoi(yytext); return NUMBER;}
                
%%


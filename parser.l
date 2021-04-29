%{
    #include "grammar.tab.h"
%}

/* regular definitions */
delim   [ \t\n]
ws      {delim}+
letter  [A-Za-z]
digit   [0-9]
number  [0-9]+
id      {letter}({letter}|{digit})*

%%
    /* whitespace */
{ws}      {/* do nothing with whitespace */}

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
"!="      {return OP_NOT_EQUAL;}
"!"       {return OP_INVERT;}
"&&"      {return OP_LOG_AND;}
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
{id}      {return ID;}
{number}  {return NUMBER;}

%%


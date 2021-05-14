%{
    #include "grammar.tab.h"
%}

/* regular definitions */
delim   [ \t\n]
ws      {delim}+
letter  [A-Za-z_]
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
"!"       {yylval.unop_dec = new Unop_dec(yytext); return OP_INVERT;}
"!="      {yylval.binop_dec = new Binop_dec(yytext);return OP_NOT_EQUAL;}
"&&"      {yylval.binop_dec = new Binop_dec(yytext);return OP_LOG_AND;}
"*"       {yylval.binop_dec = new Binop_dec(yytext);return OP_MULT;}
"+"       {yylval.binop_dec = new Binop_dec(yytext);return OP_ADD;}
"-"       {yylval.binop_dec = new Binop_dec(yytext);return OP_MINUS;}
"/"       {yylval.binop_dec = new Binop_dec(yytext);return OP_DIVIDE;}
"<="      {yylval.binop_dec = new Binop_dec(yytext);return OP_LESS_EQUAL;}
"<"       {yylval.binop_dec = new Binop_dec(yytext);return OP_LESS_THAN;}
"=="      {yylval.binop_dec = new Binop_dec(yytext);return OP_EQUAL;}
"="       {yylval.binop_dec = new Binop_dec(yytext);return OP_ASSIGN;}
">="      {yylval.binop_dec = new Binop_dec(yytext);return OP_GREATER_EQUAL;}
">"       {yylval.binop_dec = new Binop_dec(yytext);return OP_GREATER_THAN;}

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


%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "parser.tab.h"

int line_num = 1;
extern int lex_only;

// Macro to print tokens with ANSI colors
#define LOG_TOKEN(type) do { \
    printf("\033[1;30m[Line %-2d]\033[0m \033[1;34m%-15s\033[0m : \033[1;32m%s\033[0m\n", line_num, type, yytext); \
} while(0)

void yyerror(const char* s);
%}

%option noyywrap

DIGIT       [0-9]
LETTER      [a-zA-Z_]
ID          {LETTER}({LETTER}|{DIGIT})*
NUMBER      {DIGIT}+

%%

"/*"([^*]|\*+[^*/])*\*+"/"   { /* Ignore */ }
"//".*                       { /* Ignore */ }

[ \t\r]+                     { /* Ignore whitespace */ }
\n                           { line_num++; }

"int"                        { LOG_TOKEN("KEYWORD(int)"); return INT; }
"float"                      { LOG_TOKEN("KEYWORD(float)"); return FLOAT; }
"char"                       { LOG_TOKEN("KEYWORD(char)"); return CHAR; }
"void"                       { LOG_TOKEN("KEYWORD(void)"); return VOID; }

{NUMBER}                     {
                                 LOG_TOKEN("NUMBER");
                                 yylval.str = strdup(yytext);
                                 return NUMBER;
                             }
{ID}                         {
                                 LOG_TOKEN("IDENTIFIER");
                                 yylval.str = strdup(yytext);
                                 return IDENTIFIER;
                             }

"++"                         { LOG_TOKEN("OPERATOR(++)"); return INC; }
"--"                         { LOG_TOKEN("OPERATOR(--)"); return DEC; }
"&&"                         { LOG_TOKEN("OPERATOR(&&)"); return AND; }
"||"                         { LOG_TOKEN("OPERATOR(||)"); return OR; }
"=="                         { LOG_TOKEN("OPERATOR(==)"); return EQ; }
"!="                         { LOG_TOKEN("OPERATOR(!=)"); return NEQ; }
"<="                         { LOG_TOKEN("OPERATOR(<=)"); return LEQ; }
">="                         { LOG_TOKEN("OPERATOR(>=)"); return GEQ; }

"="                          { LOG_TOKEN("ASSIGN(=)"); return '='; }
"+"                          { LOG_TOKEN("OPERATOR(+)"); return '+'; }
"-"                          { LOG_TOKEN("OPERATOR(-)"); return '-'; }
"*"                          { LOG_TOKEN("OPERATOR(*)"); return '*'; }
"/"                          { LOG_TOKEN("OPERATOR(/)"); return '/'; }
";"                          { LOG_TOKEN("SEMICOLON"); return ';'; }
"("                          { LOG_TOKEN("PAREN_OPEN"); return '('; }
")"                          { LOG_TOKEN("PAREN_CLOSE"); return ')'; }
"{"                          { LOG_TOKEN("BRACE_OPEN"); return '{'; }
"}"                          { LOG_TOKEN("BRACE_CLOSE"); return '}'; }

.                            { 
                                 fprintf(stderr, "\033[1;31mWarning:\033[0m Unrecognized character '%s' at line %d, skipping.\n", yytext, line_num); 
                             }

%%

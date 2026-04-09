%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

extern int yylex();
extern int line_num;
extern char* yytext;
void yyerror(const char* s);

Node* root = NULL;
%}

%union {
    char* str;
    struct Node* node;
}

%token <str> IDENTIFIER NUMBER
%token INT FLOAT CHAR VOID
%token INC DEC AND OR EQ NEQ LEQ GEQ

%type <node> program statement_list statement var_decl expr assign_stmt type

%left OR
%left AND
%left EQ NEQ
%left '<' '>' LEQ GEQ
%left '+' '-'
%left '*' '/'
%right INC DEC

%%

program:
    statement_list { root = $1; }
    ;

statement_list:
    statement { $$ = $1; }
    | statement_list statement { $$ = create_list_node($1, $2); }
    ;

statement:
    var_decl { $$ = $1; }
    | assign_stmt { $$ = $1; }
    | expr ';' { $$ = create_node(NODE_EXPR_STMT, NULL, $1, NULL); }
    | error ';' { 
        yyerrok; 
        $$ = NULL; // Simple error recovery: skip ahead to next statement upon bad syntax
    }
    ;

var_decl:
    type IDENTIFIER ';' {
        Node* id = create_node(NODE_IDENTIFIER, $2, NULL, NULL);
        $$ = create_node(NODE_VAR_DECL, NULL, $1, id);
    }
    | type IDENTIFIER '=' expr ';' {
        Node* id = create_node(NODE_IDENTIFIER, $2, NULL, NULL);
        Node* assign = create_node(NODE_ASSIGN, "=", id, $4);
        $$ = create_node(NODE_VAR_DECL, NULL, $1, assign);
    }
    ;

type:
    INT { $$ = create_node(NODE_TYPE_INT, NULL, NULL, NULL); }
    | FLOAT { $$ = create_node(NODE_TYPE_FLOAT, NULL, NULL, NULL); }
    | CHAR { $$ = create_node(NODE_TYPE_CHAR, NULL, NULL, NULL); }
    | VOID { $$ = create_node(NODE_TYPE_VOID, NULL, NULL, NULL); }
    ;

assign_stmt:
    IDENTIFIER '=' expr ';' {
        Node* id = create_node(NODE_IDENTIFIER, $1, NULL, NULL);
        $$ = create_node(NODE_ASSIGN, "=", id, $3);
    }
    ;

expr:
    NUMBER { $$ = create_node(NODE_NUMBER, $1, NULL, NULL); }
    | IDENTIFIER { $$ = create_node(NODE_IDENTIFIER, $1, NULL, NULL); }
    | expr '+' expr { $$ = create_node(NODE_BINOP, "+", $1, $3); }
    | expr '-' expr { $$ = create_node(NODE_BINOP, "-", $1, $3); }
    | expr '*' expr { $$ = create_node(NODE_BINOP, "*", $1, $3); }
    | expr '/' expr { $$ = create_node(NODE_BINOP, "/", $1, $3); }
    | expr AND expr { $$ = create_node(NODE_BINOP, "&&", $1, $3); }
    | expr OR expr { $$ = create_node(NODE_BINOP, "||", $1, $3); }
    | expr EQ expr { $$ = create_node(NODE_BINOP, "==", $1, $3); }
    | expr NEQ expr { $$ = create_node(NODE_BINOP, "!=", $1, $3); }
    | INC IDENTIFIER {
        Node* id = create_node(NODE_IDENTIFIER, $2, NULL, NULL);
        $$ = create_node(NODE_UNOP, "++(pre)", id, NULL);
    }
    | IDENTIFIER INC {
        Node* id = create_node(NODE_IDENTIFIER, $1, NULL, NULL);
        $$ = create_node(NODE_UNOP, "(post)++", id, NULL);
    }
    | DEC IDENTIFIER {
        Node* id = create_node(NODE_IDENTIFIER, $2, NULL, NULL);
        $$ = create_node(NODE_UNOP, "--(pre)", id, NULL);
    }
    | IDENTIFIER DEC {
        Node* id = create_node(NODE_IDENTIFIER, $1, NULL, NULL);
        $$ = create_node(NODE_UNOP, "(post)--", id, NULL);
    }
    | '(' expr ')' { $$ = $2; }
    ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "Syntax Error at line %d: %s\n", line_num, s);
}

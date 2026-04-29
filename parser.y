%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

extern int yylex(); //will call lexer
extern int line_num; // will track line NUMBER
extern char* yytext;    //current token text
void yyerror(const char* s); //will call error function

Node* root = NULL;
int parse_error_count = 0;
extern char* yytext;
extern int line_num;

void print_detailed_error(const char* title, const char* hint);
%}

%define parse.error verbose

%union {    //datatype for tokens
    char* str;
    struct Node* node;
}

%token <str> IDENTIFIER NUMBER
%token INT FLOAT CHAR VOID
%token IF ELSE WHILE FOR RETURN BREAK
%token INC DEC AND OR EQ NEQ LEQ GEQ

%type <node> program statement_list statement var_decl expr assign_stmt type
%type <node> block opt_expr func_decl assign_stmt_no_semi

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%left OR
%left AND
%left EQ NEQ
%left '<' '>' LEQ GEQ
%left '+' '-'
%left '*' '/'
%right INC DEC

%%

program:
    statement_list { root = create_node(NODE_PROGRAM, NULL, $1, NULL); }
    ;

statement_list:
    statement { $$ = $1; }
    | statement_list statement { $$ = create_list_node($1, $2); }
    ;

block:
    '{' statement_list '}' { $$ = create_node(NODE_BLOCK, NULL, $2, NULL); }
    | '{' '}' { $$ = create_node(NODE_BLOCK, NULL, NULL, NULL); }
    ;

statement:
    var_decl { $$ = $1; }
    | assign_stmt { $$ = create_node(NODE_EXPR_STMT, NULL, $1, NULL); }
    | func_decl { $$ = $1; }
    | expr ';' { $$ = create_node(NODE_EXPR_STMT, NULL, $1, NULL); }
    | block { $$ = $1; }
    | IF '(' expr ')' statement %prec LOWER_THAN_ELSE { 
        $$ = create_node_3(NODE_IF, NULL, $3, $5, NULL); 
    }
    | IF '(' expr ')' statement ELSE statement { 
        $$ = create_node_3(NODE_IF, NULL, $3, $5, $7); 
    }
    | WHILE '(' expr ')' statement { 
        $$ = create_node(NODE_WHILE, NULL, $3, $5); 
    }
    | FOR '(' opt_expr ';' opt_expr ';' opt_expr ')' statement {
        Node* inc_block = create_node(NODE_PROGRAM, NULL, $7, $9); 
        $$ = create_node_3(NODE_FOR, NULL, $3, $5, inc_block);
    }
    | FOR '(' error ')' statement {
        print_detailed_error("Incomplete for-loop statement", "Expected:\nfor(initialization; condition; increment)\n\nExample:\nfor(i = 0; i < 10; i++)");
        $$ = NULL;
    }
    | FOR '(' ')' ';' {
        print_detailed_error("Incomplete for-loop statement", "Expected:\nfor(initialization; condition; increment)\n\nExample:\nfor(i = 0; i < 10; i++)");
        $$ = NULL;
    }
    | RETURN opt_expr ';' { $$ = create_node(NODE_RETURN, NULL, $2, NULL); }
    | BREAK ';' { $$ = create_node(NODE_BREAK, NULL, NULL, NULL); }
    | error ';' { 
        yyerrok; 
        $$ = NULL; // Simple error recovery: skip ahead to next statement upon bad syntax
    }
    ;

opt_expr:
    /* empty */ { $$ = NULL; }
    | expr { $$ = $1; }
    | assign_stmt_no_semi { $$ = $1; }
    ;

assign_stmt_no_semi:
    IDENTIFIER '=' expr {
        Node* id = create_node(NODE_IDENTIFIER, $1, NULL, NULL);
        $$ = create_node(NODE_ASSIGN, "=", id, $3);
    }
    ;

func_decl:
    type IDENTIFIER '(' ')' block {
        Node* id = create_node(NODE_IDENTIFIER, $2, NULL, NULL);
        $$ = create_node_3(NODE_FUNC_DECL, NULL, $1, id, $5);
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
    | type IDENTIFIER '=' error ';' {
        print_detailed_error("Missing expression after '='", "Ensure the variable is assigned a valid value.");
        $$ = NULL;
    }
    | type IDENTIFIER '[' NUMBER ']' ';' {
        Node* id = create_node(NODE_IDENTIFIER, $2, NULL, NULL);
        Node* num = create_node(NODE_NUMBER, $4, NULL, NULL);
        $$ = create_node_3(NODE_ARRAY_DECL, NULL, $1, id, num);
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
    | expr '<' expr { $$ = create_node(NODE_BINOP, "<", $1, $3); }
    | expr '>' expr { $$ = create_node(NODE_BINOP, ">", $1, $3); }
    | expr LEQ expr { $$ = create_node(NODE_BINOP, "<=", $1, $3); }
    | expr GEQ expr { $$ = create_node(NODE_BINOP, ">=", $1, $3); }
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

void print_detailed_error(const char* title, const char* hint) {
    fprintf(stderr, "\n\033[1;31mSyntax Error:\033[0m\n");
    fprintf(stderr, "%s at line %d\n", title, line_num);
    if (hint) {
        fprintf(stderr, "\n\033[1;33mHint:\033[0m\n%s\n", hint);
    }
    parse_error_count++;
}

void yyerror(const char* s) {
    // If yytext is empty, we might be at EOF
    int is_eof = (yytext == NULL || yytext[0] == '\0');
    
    // Intercept specific generic verbose errors
    if (strstr(s, "unexpected end of file") || (is_eof && strstr(s, "expected"))) {
        print_detailed_error("Missing closing brace '}' before end of file", "Check your if-statements, loops, and functions to ensure all '{' have a matching '}'.");
        return;
    }
    
    if (strstr(s, "unexpected '*'") || strstr(s, "unexpected '+'") || strstr(s, "unexpected '-'") || strstr(s, "unexpected '/'")) {
        char title[128];
        snprintf(title, sizeof(title), "Invalid expression near '%s'", yytext);
        print_detailed_error(title, "Expected a valid operand (variable or number).");
        return;
    }
    
    if (strstr(s, "unexpected ';'") && strstr(s, "expected")) {
        char title[128];
        snprintf(title, sizeof(title), "Unexpected token ';' near '%s'", yytext);
        print_detailed_error(title, "You may be missing an expression before the semicolon.");
        return;
    }

    // Default colorful error
    fprintf(stderr, "\n\033[1;31mSyntax Error at line %d:\033[0m\n", line_num);
    fprintf(stderr, "%s near token '\033[1;31m%s\033[0m'\n", s, yytext);
    parse_error_count++;
}

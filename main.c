#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

#ifdef _WIN32
#include <windows.h>
#endif

extern int yyparse();
extern int yylex();
extern FILE* yyin;
extern Node* root;

int lex_only = 0; // Global flag for lex-only visualization mode

int main(int argc, char** argv) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    if (argc < 2) {
        printf("Usage: %s [--lex] <source_file>\n", argv[0]);
        return 1;
    }

    const char* filename = argv[1];
    
    // Check for our debug flag
    if (argc == 3 && strcmp(argv[1], "--lex") == 0) {
        lex_only = 1;
        filename = argv[2];
    } else if (argc == 3) {
        printf("Usage: %s [--lex] <source_file>\n", argv[0]);
        return 1;
    }

    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Could not open file: %s\n", filename);
        return 1;
    }

    yyin = file;
    
    printf("\n\033[1;33m--- [Compiler Optimizer & Visualizer Phase 2] ---\033[0m\n");
    printf("Starting compilation for \033[1;35m%s\033[0m...\n\n", filename);
    
    if (lex_only) {
        printf("\033[1;36m*** LEXER DEBUG MODE ENGAGED ***\033[0m\n");
        printf("Tokenizing without building AST.\n--------------------------------------\n");
        while(yylex()) {
            // yylex loop will just trigger our lexer macros to print all tokens!
        }
        printf("--------------------------------------\n");
        printf("Tokens successfully extracted.\n");
    } else {
        printf("\033[1;36m*** AST GENERATION MODE ***\033[0m\n");
        printf("Parsing Tokens & Reducing Grammar...\n");
        printf("--------------------------------------\n");
        int result = yyparse();
        
        printf("\n\033[1;32m=== Abstract Syntax Tree ===\033[0m\n");
        if (root) {
            print_ast(root, 0);
            free_ast(root);
        } else {
            printf("(Empty or failed AST)\n");
        }
        
        printf("\n=== Target Code Optimization ===\n");
        printf("[\033[1;33mPending for Phase 3\033[0m]\n");
        
        printf("\n=== Target Code Generation ===\n");
        printf("[\033[1;33mPending for Phase 3\033[0m]\n\n");
        
        fclose(file);
        return result;
    }

    fclose(file);
    return 0;
}

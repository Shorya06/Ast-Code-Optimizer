#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "optimizer.h"
#include "codegen.h"

#ifdef _WIN32
#include <windows.h>
#endif

extern int yyparse();
extern int yylex();
extern FILE* yyin;
extern Node* root;
extern int parse_error_count;

int lex_only = 0; // Global flag for lex only visualization mode

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
        
        if (parse_error_count > 0) {
            printf("\n\033[1;31m=== PARSER ERROR ===\033[0m\n");
            printf("Optimization aborted.\n");
            printf("Code generation aborted.\n");
            fclose(file);
            return 1;
        }
        
        printf("\n\033[1;32m=== AST BEFORE OPTIMIZATION ===\033[0m\n");
        if (root) {
            print_ast(root, 0);
            
            generate_dot_file(root, "AST_before.dot");
            
            // Optimization Phase
            root = optimize_ast(root);
            
            printf("\n\033[1;32m=== AST AFTER OPTIMIZATION ===\033[0m\n");
            print_ast(root, 0);
            
            generate_dot_file(root, "AST_after.dot");
            
            printf("\n=== GENERATED OPTIMIZED CODE ===\n");
            FILE* out_file = fopen("optimized_output.c", "w");
            if (out_file) {
                generate_code(root, out_file);
                fclose(out_file);
                printf("[\033[1;32m✓\033[0m] Code successfully written to 'optimized_output.c'\n");
            } else {
                fprintf(stderr, "[\033[1;31mX\033[0m] Failed to open optimized_output.c for writing\n");
            }
            
            // Print generated code to console as well
            generate_code(root, stdout);
            printf("\n");
            
            free_ast(root);
        } else {
            printf("(Empty or failed AST)\n");
        }
        
        fclose(file);
        return result;
    }

    fclose(file);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

Node* create_node(NodeType type, const char* value, Node* left, Node* right) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (!node) {
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }
    node->type = type;
    node->value = value ? strdup(value) : NULL;
    node->left = left;
    node->right = right;
    node->next = NULL;
    return node;
}

Node* create_list_node(Node* current, Node* next) {
    if (!current) return next;
    Node* temp = current;
    while (temp->next) {
        temp = temp->next;
    }
    temp->next = next;
    return current;
}

static const char* type_to_string(NodeType type) {
    switch(type) {
        case NODE_PROGRAM: return "Program";
        case NODE_VAR_DECL: return "VariableDeclaration";
        case NODE_ASSIGN: return "Assignment";
        case NODE_BINOP: return "BinaryOperation";
        case NODE_UNOP: return "UnaryOperation";
        case NODE_IDENTIFIER: return "Identifier";
        case NODE_NUMBER: return "NumberLiteral";
        case NODE_EXPR_STMT: return "ExpressionStatement";
        case NODE_TYPE_INT: return "Type(int)";
        case NODE_TYPE_FLOAT: return "Type(float)";
        case NODE_TYPE_CHAR: return "Type(char)";
        case NODE_TYPE_VOID: return "Type(void)";
        default: return "Unknown";
    }
}

static void print_ast_internal(Node* node, int depth, int* is_parent_last, int is_last) {
    if (!node) return;

    for (int i = 0; i < depth; i++) {
        if (is_parent_last[i]) {
            printf("    ");
        } else {
            printf("│   ");
        }
    }

    printf(is_last ? "└── " : "├── ");

    printf("\033[1;36m%s\033[0m", type_to_string(node->type));
    if (node->value) {
        printf(" [\033[1;32m%s\033[0m]", node->value);
    }
    printf("\n");

    is_parent_last[depth] = is_last;

    int has_left = (node->left != NULL);
    int has_right = (node->right != NULL);

    if (has_left) {
        print_ast_internal(node->left, depth + 1, is_parent_last, !has_right);
    }
    if (has_right) {
        print_ast_internal(node->right, depth + 1, is_parent_last, 1);
    }
}

void print_ast(Node* node, int depth) {
    int is_parent_last[1024] = {0};
    Node* curr = node;
    while (curr) {
        int is_last = (curr->next == NULL);
        print_ast_internal(curr, depth, is_parent_last, is_last);
        curr = curr->next;
    }
}

void free_ast(Node* node) {
    if (!node) return;
    free_ast(node->left);
    free_ast(node->right);
    free_ast(node->next);
    if (node->value) free(node->value);
    free(node);
}

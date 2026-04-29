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
    node->middle = NULL;
    node->right = right;
    node->next = NULL;
    return node;
}

Node* create_node_3(NodeType type, const char* value, Node* left, Node* middle, Node* right) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (!node) {
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }
    node->type = type;
    node->value = value ? strdup(value) : NULL;
    node->left = left;
    node->middle = middle;
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
        case NODE_IF: return "IfStatement";
        case NODE_WHILE: return "WhileLoop";
        case NODE_DO_WHILE: return "DoWhileLoop";
        case NODE_FOR: return "ForLoop";
        case NODE_RETURN: return "Return";
        case NODE_BLOCK: return "Block";
        case NODE_FUNC_DECL: return "FunctionDeclaration";
        case NODE_ARRAY_DECL: return "ArrayDeclaration";
        case NODE_BREAK: return "Break";
        case NODE_SWITCH: return "Switch";
        case NODE_CASE: return "Case";
        case NODE_DEFAULT: return "Default";
        default: return "Unknown";
    }
}

static void print_ast_internal(Node* node, int depth, int* is_parent_last, int is_last, FILE* out) {
    if (!node) return;

    for (int i = 0; i < depth; i++) {
        if (is_parent_last[i]) {
            fprintf(out, "    ");
        } else {
            fprintf(out, "│   ");
        }
    }

    fprintf(out, is_last ? "└── " : "├── ");

    fprintf(out, "\033[1;36m%s\033[0m", type_to_string(node->type));
    if (node->value) {
        fprintf(out, " [\033[1;32m%s\033[0m]", node->value);
    }
    fprintf(out, "\n");

    is_parent_last[depth] = is_last;

    int has_left = (node->left != NULL);
    int has_middle = (node->middle != NULL);
    int has_right = (node->right != NULL);

    Node* curr;
    
    curr = node->left;
    while (curr) {
        int is_last = (curr->next == NULL && !has_middle && !has_right);
        print_ast_internal(curr, depth + 1, is_parent_last, is_last, out);
        curr = curr->next;
    }

    curr = node->middle;
    while (curr) {
        int is_last = (curr->next == NULL && !has_right);
        print_ast_internal(curr, depth + 1, is_parent_last, is_last, out);
        curr = curr->next;
    }

    curr = node->right;
    while (curr) {
        int is_last = (curr->next == NULL);
        print_ast_internal(curr, depth + 1, is_parent_last, is_last, out);
        curr = curr->next;
    }
}

void print_ast(Node* node, int depth, FILE* out) {
    int is_parent_last[1024] = {0};
    Node* curr = node;
    while (curr) {
        int is_last = (curr->next == NULL);
        print_ast_internal(curr, depth, is_parent_last, is_last, out);
        curr = curr->next;
    }
}

void free_ast(Node* node) {
    if (!node) return;
    free_ast(node->left);
    free_ast(node->middle);
    free_ast(node->right);
    free_ast(node->next);
    if (node->value) free(node->value);
    free(node);
}

static int dot_node_id = 0;
static void generate_dot_file_internal(Node* node, FILE* out, int* parent_id) {
    if (!node) return;
    int current_id = dot_node_id++;
    
    fprintf(out, "  node%d [label=\"%s", current_id, type_to_string(node->type));
    if (node->value) {
        // Escape quotes
        fprintf(out, "\\n");
        for(int i = 0; node->value[i]; i++) {
            if(node->value[i] == '"') fprintf(out, "\\\"");
            else fprintf(out, "%c", node->value[i]);
        }
    }
    fprintf(out, "\"];\n");
    
    if (parent_id) {
        fprintf(out, "  node%d -> node%d;\n", *parent_id, current_id);
    }
    
    if (node->left) generate_dot_file_internal(node->left, out, &current_id);
    if (node->middle) generate_dot_file_internal(node->middle, out, &current_id);
    if (node->right) generate_dot_file_internal(node->right, out, &current_id);
    if (node->next) {
        // For NEXT links, we could show them as dotted lines
        int next_id = dot_node_id;
        generate_dot_file_internal(node->next, out, NULL);
        fprintf(out, "  node%d -> node%d [style=dotted, label=\"next\"];\n", current_id, next_id);
    }
}

void generate_dot_file(Node* node, const char* filename) {
    if (!node) return;
    FILE* out = fopen(filename, "w");
    if (!out) {
        fprintf(stderr, "Could not open %s for writing\n", filename);
        return;
    }
    fprintf(out, "digraph AST {\n");
    fprintf(out, "  node [shape=box, style=filled, fillcolor=lightblue, fontname=\"Arial\"];\n");
    dot_node_id = 0;
    generate_dot_file_internal(node, out, NULL);
    fprintf(out, "}\n");
    fclose(out);
}

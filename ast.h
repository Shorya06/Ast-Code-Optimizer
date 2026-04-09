#ifndef AST_H
#define AST_H

typedef enum {
    NODE_PROGRAM,
    NODE_VAR_DECL,
    NODE_ASSIGN,
    NODE_BINOP,
    NODE_UNOP,
    NODE_IDENTIFIER,
    NODE_NUMBER,
    NODE_EXPR_STMT,
    NODE_TYPE_INT,
    NODE_TYPE_FLOAT,
    NODE_TYPE_CHAR,
    NODE_TYPE_VOID
} NodeType;

typedef struct Node {
    NodeType type;
    char* value;
    struct Node* left;
    struct Node* right;
    struct Node* next;
} Node;

Node* create_node(NodeType type, const char* value, Node* left, Node* right);
Node* create_list_node(Node* current, Node* next);
void print_ast(Node* node, int depth);
void free_ast(Node* node);

#endif

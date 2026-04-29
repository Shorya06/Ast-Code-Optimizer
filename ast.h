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
    NODE_TYPE_VOID,
    NODE_IF,
    NODE_WHILE,
    NODE_DO_WHILE,
    NODE_FOR,
    NODE_RETURN,
    NODE_BLOCK,
    NODE_FUNC_DECL,
    NODE_ARRAY_DECL,
    NODE_BREAK,
    NODE_SWITCH,
    NODE_CASE,
    NODE_DEFAULT
} NodeType;

typedef struct Node {
    NodeType type;
    char* value;
    struct Node* left;
    struct Node* middle; /* For if-else, for loops, etc */
    struct Node* right;
    struct Node* next;
} Node;

Node* create_node(NodeType type, const char* value, Node* left, Node* right);
Node* create_node_3(NodeType type, const char* value, Node* left, Node* middle, Node* right);
Node* create_list_node(Node* current, Node* next);
void print_ast(Node* node, int depth, FILE* out);
void free_ast(Node* node);
void generate_dot_file(Node* node, const char* filename);

#endif

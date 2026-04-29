#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"

static void indent(FILE* out, int level) {
    for(int i = 0; i < level; i++) {
        fprintf(out, "    ");
    }
}

static void generate_code_internal(Node* node, FILE* out, int level) {
    if (!node) return;

    switch (node->type) {
        case NODE_PROGRAM:
        case NODE_BLOCK:
            // blocks usually contain a list of statements
            {
                Node* curr = node->left; 
                if (node->type != NODE_PROGRAM) {
                    indent(out, level);
                    fprintf(out, "{\n");
                }
                
                while (curr) {
                    generate_code_internal(curr, out, level + (node->type == NODE_BLOCK ? 1 : 0));
                    curr = curr->next;
                }
                
                if (node->type == NODE_BLOCK) {
                    indent(out, level);
                    fprintf(out, "}\n");
                } else {
                    return; // Avoid printing next if program
                }
            }
            break;
            
        case NODE_FUNC_DECL:
            fprintf(out, "\n");
            indent(out, level);
            // left: type, middle: id, right: block
            generate_code_internal(node->left, out, 0); // type
            fprintf(out, " ");
            generate_code_internal(node->middle, out, 0); // id
            fprintf(out, "() "); // For now, empty params
            if (node->right) {
                generate_code_internal(node->right, out, level); // block
            } else {
                fprintf(out, ";\n");
            }
            break;

        case NODE_VAR_DECL:
            indent(out, level);
            generate_code_internal(node->left, out, 0); // Type
            fprintf(out, " ");
            generate_code_internal(node->right, out, 0); // Identifier or Assignment
            fprintf(out, ";\n");
            break;

        case NODE_ASSIGN:
            if (level > 0) indent(out, level);
            generate_code_internal(node->left, out, 0);
            fprintf(out, " %s ", node->value ? node->value : "=");
            generate_code_internal(node->right, out, 0);
            if (level > 0) fprintf(out, ";\n");
            break;

        case NODE_BINOP:
            fprintf(out, "(");
            generate_code_internal(node->left, out, 0);
            fprintf(out, " %s ", node->value);
            generate_code_internal(node->right, out, 0);
            fprintf(out, ")");
            break;

        case NODE_UNOP:
            if (strcmp(node->value, "++(pre)") == 0 || strcmp(node->value, "--(pre)") == 0) {
                fprintf(out, "%s", node->value[0] == '+' ? "++" : "--");
                generate_code_internal(node->left, out, 0);
            } else {
                generate_code_internal(node->left, out, 0);
                fprintf(out, "%s", node->value[6] == '+' ? "++" : "--");
            }
            break;

        case NODE_IDENTIFIER:
        case NODE_NUMBER:
            fprintf(out, "%s", node->value);
            break;

        case NODE_EXPR_STMT:
            indent(out, level);
            generate_code_internal(node->left, out, 0);
            fprintf(out, ";\n");
            break;

        case NODE_TYPE_INT: fprintf(out, "int"); break;
        case NODE_TYPE_FLOAT: fprintf(out, "float"); break;
        case NODE_TYPE_CHAR: fprintf(out, "char"); break;
        case NODE_TYPE_VOID: fprintf(out, "void"); break;

        case NODE_IF:
            indent(out, level);
            fprintf(out, "if (");
            generate_code_internal(node->left, out, 0); // Condition
            fprintf(out, ") ");
            if (node->middle) {
                if (node->middle->type != NODE_BLOCK) fprintf(out, "\n");
                generate_code_internal(node->middle, out, node->middle->type != NODE_BLOCK ? level + 1 : level);
            } else {
                fprintf(out, "{}\n");
            }
            
            if (node->right) {
                indent(out, level);
                fprintf(out, "else ");
                if (node->right->type != NODE_BLOCK && node->right->type != NODE_IF) fprintf(out, "\n");
                generate_code_internal(node->right, out, (node->right->type != NODE_BLOCK && node->right->type != NODE_IF) ? level + 1 : level);
            }
            break;

        case NODE_WHILE:
            indent(out, level);
            fprintf(out, "while (");
            generate_code_internal(node->left, out, 0);
            fprintf(out, ") ");
            if (node->right) {
                if (node->right->type != NODE_BLOCK) fprintf(out, "\n");
                generate_code_internal(node->right, out, node->right->type != NODE_BLOCK ? level + 1 : level);
            } else {
                fprintf(out, "{}\n");
            }
            break;

        case NODE_FOR:
            indent(out, level);
            fprintf(out, "for (");
            generate_code_internal(node->left, out, 0); // init
            fprintf(out, "; ");
            generate_code_internal(node->middle, out, 0); // cond
            fprintf(out, "; ");
            // the inc part is usually stored in right's left or similar, let's assume right is block and inc is stored somehow.
            // Wait, for loops need 4 parts. Node only has left, middle, right.
            // I'll structure FOR as: left=init, middle=cond, right=list(inc, block) where right->left=inc, right->right=block.
            if (node->right && node->right->type == NODE_PROGRAM) {
                generate_code_internal(node->right->left, out, 0); // inc
                fprintf(out, ") ");
                generate_code_internal(node->right->right, out, level); // block
            } else {
                fprintf(out, ") {}\n");
            }
            break;
            
        case NODE_RETURN:
            indent(out, level);
            fprintf(out, "return");
            if (node->left) {
                fprintf(out, " ");
                generate_code_internal(node->left, out, 0);
            }
            fprintf(out, ";\n");
            break;

        default:
            fprintf(out, "/* Unknown Node %d */", node->type);
            break;
    }

    // No recursive call to node->next here, it's handled by the parent block/list
}

void generate_code(Node* root, FILE* out) {
    if (!root) return;
    generate_code_internal(root, out, 0);
}

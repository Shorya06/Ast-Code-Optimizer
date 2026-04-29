#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "optimizer.h"

static int opt_constant_folded = 0;
static int opt_strength_reduced = 0;
static int opt_dead_code_eliminated = 0;
static int opt_constant_propagated = 0;

typedef struct ConstSym {
    char* name;
    int value;
    int is_constant;
    int reassigned;
    struct ConstSym* next;
} ConstSym;

static ConstSym* sym_table = NULL;

static ConstSym* get_sym(const char* name) {
    if (!name) return NULL;
    ConstSym* curr = sym_table;
    while (curr) {
        if (strcmp(curr->name, name) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

static void register_var(const char* name, int is_const, int val) {
    if (!name) return;
    ConstSym* curr = get_sym(name);
    if (curr) {
        // Redefined? Mark as reassigned just in case
        curr->reassigned = 1;
        return;
    }
    ConstSym* nu = (ConstSym*)malloc(sizeof(ConstSym));
    nu->name = strdup(name);
    nu->value = val;
    nu->is_constant = is_const;
    nu->reassigned = 0;
    nu->next = sym_table;
    sym_table = nu;
}

static void mark_reassigned(const char* name) {
    if (!name) return;
    ConstSym* sym = get_sym(name);
    if (sym) sym->reassigned = 1;
}

static void free_sym_table() {
    ConstSym* curr = sym_table;
    while (curr) {
        ConstSym* temp = curr;
        curr = curr->next;
        free(temp->name);
        free(temp);
    }
    sym_table = NULL;
}

static void collect_usage(Node* node) {
    if (!node) return;
    
    if (node->type == NODE_VAR_DECL) {
        Node* id_node = node->right;
        if (id_node && id_node->type == NODE_ASSIGN) {
            Node* lhs = id_node->left;
            Node* rhs = id_node->right;
            if (lhs && lhs->type == NODE_IDENTIFIER) {
                if (rhs && rhs->type == NODE_NUMBER) {
                    register_var(lhs->value, 1, atoi(rhs->value));
                } else {
                    register_var(lhs->value, 0, 0);
                }
            }
            collect_usage(rhs);
        } else if (id_node && id_node->type == NODE_IDENTIFIER) {
            register_var(id_node->value, 0, 0);
        }
    } else if (node->type == NODE_ASSIGN) {
        if (node->left && node->left->type == NODE_IDENTIFIER) {
            mark_reassigned(node->left->value);
        }
        collect_usage(node->right);
    } else if (node->type == NODE_UNOP) {
        if (node->left && node->left->type == NODE_IDENTIFIER) {
            mark_reassigned(node->left->value);
        }
    } else {
        collect_usage(node->left);
        collect_usage(node->middle);
        collect_usage(node->right);
    }
    
    collect_usage(node->next);
}

static int has_side_effects(Node* node) {
    if (!node) return 0;
    if (node->type == NODE_FUNC_DECL || node->type == NODE_UNOP || node->type == NODE_ASSIGN) return 1;
    return has_side_effects(node->left) || has_side_effects(node->middle) || has_side_effects(node->right);
}

static Node* constant_propagation(Node* node) {
    if (!node) return NULL;

    // Do not propagate into the LHS of an assignment
    if (node->type == NODE_ASSIGN) {
        node->right = constant_propagation(node->right);
        if (node->next) node->next = constant_propagation(node->next);
        return node;
    }

    if (node->type == NODE_VAR_DECL) {
        if (node->right && node->right->type == NODE_ASSIGN) {
            node->right->right = constant_propagation(node->right->right);
        }
        if (node->next) node->next = constant_propagation(node->next);
        return node;
    }

    if (node->type == NODE_UNOP) {
        // LHS is modified, do not propagate into it
        if (node->next) node->next = constant_propagation(node->next);
        return node;
    }

    if (node->type == NODE_IDENTIFIER) {
        ConstSym* sym = get_sym(node->value);
        if (sym && sym->is_constant && !sym->reassigned) {
            opt_constant_propagated++;
            char buf[32];
            snprintf(buf, sizeof(buf), "%d", sym->value);
            printf("[\033[1;36m✓\033[0m] \033[1;36mConstant propagated: %s → %d\033[0m\n", sym->name, sym->value);
            Node* num_node = create_node(NODE_NUMBER, buf, NULL, NULL);
            // DO NOT copy next here, since IDENTIFIERs in expressions don't use next.
            // If they did, it would be handled below.
            return num_node;
        }
    }

    node->left = constant_propagation(node->left);
    node->middle = constant_propagation(node->middle);
    node->right = constant_propagation(node->right);

    if (node->next) {
        node->next = constant_propagation(node->next);
    }

    return node;
}

static Node* constant_fold(Node* node) {
    if (!node) return NULL;

    node->left = constant_fold(node->left);
    node->middle = constant_fold(node->middle);
    node->right = constant_fold(node->right);
    if (node->next) {
        node->next = constant_fold(node->next);
    }

    if (node->type == NODE_BINOP && node->left && node->right &&
        node->left->type == NODE_NUMBER && node->right->type == NODE_NUMBER) {
        
        int left_val = atoi(node->left->value);
        int right_val = atoi(node->right->value);
        int result = 0;
        int folded = 1;

        if (strcmp(node->value, "+") == 0) result = left_val + right_val;
        else if (strcmp(node->value, "-") == 0) result = left_val - right_val;
        else if (strcmp(node->value, "*") == 0) result = left_val * right_val;
        else if (strcmp(node->value, "<<") == 0) result = left_val << right_val;
        else if (strcmp(node->value, ">>") == 0) result = left_val >> right_val;
        else if (strcmp(node->value, "/") == 0) {
            if (right_val != 0) result = left_val / right_val;
            else folded = 0;
        } else {
            folded = 0;
        }

        if (folded) {
            char buf[32];
            snprintf(buf, sizeof(buf), "%d", result);
            Node* new_node = create_node(NODE_NUMBER, buf, NULL, NULL);
            new_node->next = node->next; // Preserve list
            
            printf("[\033[1;36m✓\033[0m] \033[1;36mFolded expression: %d %s %d → %d\033[0m\n", left_val, node->value, right_val, result);
            
            opt_constant_folded++;
            return new_node;
        }
    }

    return node;
}

static Node* strength_reduction(Node* node) {
    if (!node) return NULL;

    node->left = strength_reduction(node->left);
    node->middle = strength_reduction(node->middle);
    node->right = strength_reduction(node->right);
    if (node->next) {
        node->next = strength_reduction(node->next);
    }

    if (node->type == NODE_BINOP && node->left && node->right) {
        if (strcmp(node->value, "*") == 0) {
            if (node->right->type == NODE_NUMBER && strcmp(node->right->value, "2") == 0) {
                free(node->value);
                node->value = strdup("<<");
                free(node->right->value);
                node->right->value = strdup("1");
                opt_strength_reduced++;
            } else if (node->left->type == NODE_NUMBER && strcmp(node->left->value, "2") == 0) {
                free(node->value);
                node->value = strdup("<<");
                Node* temp = node->left;
                node->left = node->right;
                node->right = temp;
                free(node->right->value);
                node->right->value = strdup("1");
                opt_strength_reduced++;
            }
        } else if (strcmp(node->value, "/") == 0) {
            if (node->right->type == NODE_NUMBER && strcmp(node->right->value, "2") == 0) {
                free(node->value);
                node->value = strdup(">>");
                free(node->right->value);
                node->right->value = strdup("1");
                opt_strength_reduced++;
            }
        }
    }

    return node;
}

static Node* dead_code_elimination(Node* node) {
    if (!node) return NULL;

    node->left = dead_code_elimination(node->left);
    node->middle = dead_code_elimination(node->middle);
    node->right = dead_code_elimination(node->right);

    // Remove safely fully-propagated constant declarations
    if (node->type == NODE_VAR_DECL) {
        Node* id_node = node->right;
        Node* rhs = NULL;
        if (id_node && id_node->type == NODE_ASSIGN) {
            rhs = id_node->right;
            id_node = id_node->left;
        }
        
        if (id_node && id_node->type == NODE_IDENTIFIER) {
            ConstSym* sym = get_sym(id_node->value);
            if (sym && sym->is_constant && !sym->reassigned && !has_side_effects(rhs)) {
                opt_dead_code_eliminated++;
                printf("[\033[1;36m✓\033[0m] \033[1;36mRemoved unused declaration: %s\033[0m\n", sym->name);
                return dead_code_elimination(node->next); // Remove this node and continue
            }
        }
    }

    // Eliminate unreachable code after return
    if (node->type == NODE_RETURN && node->next != NULL) {
        opt_dead_code_eliminated++;
        node->next = NULL;
    }

    if (node->next) {
        node->next = dead_code_elimination(node->next);
    }

    // Eliminate empty if(0) blocks
    if (node->type == NODE_IF && node->left && node->left->type == NODE_NUMBER && strcmp(node->left->value, "0") == 0) {
        opt_dead_code_eliminated++;
        Node* next_stmt = node->next;
        Node* replacement = node->right; // else branch
        if (replacement) {
            Node* temp = replacement;
            while(temp->next) temp = temp->next;
            temp->next = next_stmt;
            return replacement;
        } else {
            return next_stmt;
        }
    }

    return node;
}

Node* optimize_ast(Node* root) {
    if (!root) return NULL;
    
    printf("\n=== RUNNING OPTIMIZATIONS ===\n");
    
    opt_constant_folded = 0;
    opt_strength_reduced = 0;
    opt_dead_code_eliminated = 0;
    opt_constant_propagated = 0;

    free_sym_table();
    // Pre-fold basic expressions so they can be recognized as constants
    root = constant_fold(root);
    collect_usage(root);

    // Apply main passes
    root = constant_propagation(root);
    root = constant_fold(root); // Fold again after propagation
    root = strength_reduction(root);
    root = constant_fold(root); // Fold again after strength reduction to catch shifts
    root = dead_code_elimination(root);
    
    free_sym_table();
    
    if (opt_constant_propagated == 0 && opt_constant_folded == 0 && opt_strength_reduced == 0 && opt_dead_code_eliminated == 0) {
        printf("[\033[1;34mi\033[0m] No optimizations could be applied.\n");
    }

    printf("\n\033[1;33m=== OPTIMIZATION SUMMARY ===\033[0m\n");
    printf("Constant Folding Applied: %d\n", opt_constant_folded);
    printf("Constant Propagation Applied: %d\n", opt_constant_propagated);
    printf("Strength Reduction Applied: %d\n", opt_strength_reduced);
    printf("Dead Code Eliminated: %d\n", opt_dead_code_eliminated);

    return root;
}

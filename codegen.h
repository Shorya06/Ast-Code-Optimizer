#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include <stdio.h>

void generate_code(Node* root, FILE* out);

#endif

/* 
 * sample2_expression.c
 *
 * Demonstrates the parser's ability to handle mathematical precedence, 
 * logical comparison ordering, and advanced token operators.
 */

int base = 50;
int multiplier = 4;
int offset = 15;

// Multiplication should take precedence over addition globally in the AST structure
int result = base + multiplier * offset;

// Logic expressions demonstrating binary node branches
int threshold = 250;
int is_valid = result >= threshold && base != 0;

int extra = is_valid || multiplier == 4;

// Pre and Post Increments properly recognized by the lexer
base++;
--offset;

// Complex single expression:
int complex = (base / 2) + (multiplier * 3) - offset;

/* 
 * sample3_invalid.c
 * 
 * Demonstrates the Lexer mapping out tokens correctly, 
 * but triggering a Graceful Error Recovery inside Bison due to invalid syntax.
 */

int valid_entry = 1;
int another_valid = 2;

// Intentional Syntax Error: assignment missing RHS value before semicolon
int broken_syntax = ;

// Another Syntax Error: mismatched / missing values
int x = + * 5;

// The compiler should skip the bad statements, log the error, 
// and gracefully recover so it doesn't crash here:
int system_recovered = 1;

valid_entry++;
another_valid--;

/* If the compiler reaches here without a fatal crash, error recovery succeeded! */

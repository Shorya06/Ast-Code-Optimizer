int a;
int b = 100000000000000000000000000000000000; // Testing very large numbers handling 
int c = a + b;

/* 
 * Testing multi-line comment.
 * This should be completely ignored by the lexer.
 * Even if it's very long...
 */

// Single line comment test

a++;
--b;

c = a && b;
a = b || c;

@ // Intentional syntax error / illegal character to test lexer recovery 

int safe = c * b / 2;

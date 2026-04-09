/* 
 * sample1_basic.c
 * 
 * Demonstration of basic lexical properties and AST struct building.
 * We include some variable declarations and basic assignments.
 */

int initialValue = 10;
float acceleration = 9; // Note: floats trigger Type(float) logic
char code = 65;

// Demonstrating standard assignments without initialization:
int x;
int y;
int z;

x = 100;
y = 200;
z = 300;

// Re-assignments
x = y;
y = z;
z = initialValue;

/* End of basic sample */

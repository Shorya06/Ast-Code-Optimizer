// Showcase: Basic Data Types, Initialization, & Abstract Syntax Tree Structure

// 1. Core Data Types and Initialization
int   starting_value = 100;
float gravity = 9;
char  letter_grade = 65;

// 2. Uninitialized Declarations
int x;
int y;
int z;

// 3. Dynamic Assignment Logic
x = starting_value + 50;
y = x * 2;
z = y / 3;

// 4. Sequential Dependencies
x = y;
y = z;
z = starting_value;

// 5. Dead Variables (Should be eliminated in optimization if unused elsewhere)
int unused_data = 999;
int redundant_data = 888;
redundant_data = unused_data;

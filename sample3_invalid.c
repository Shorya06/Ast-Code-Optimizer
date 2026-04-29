// Showcase: Professional Context-Aware Compiler Diagnostics & Error Recovery

// 1. Missing Semicolon Error
// The compiler should detect this and suggest checking for a missing semicolon.
int missing_semicolon = 10

// 2. Invalid Expression Grammar
// The compiler should catch random token mismatches.
int broken_math = * 5;

// 3. Incomplete For Loop
// The compiler should reject empty or incomplete loops with an exact expected syntax hint.
for();

// 4. Missing Right-Hand Assignment
// The compiler should prompt the user to provide a valid value after '='.
int missing_value = ;

// 5. Successful Recovery Check
// The parser should recover after logging the errors and continue evaluating the rest of the file.
int successfully_recovered = 100;
successfully_recovered++;

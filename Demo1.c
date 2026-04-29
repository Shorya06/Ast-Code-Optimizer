// Showcase: Constant Propagation, Math Folding, and Dead Code Elimination

// 1. Initial constants that should be propagated and then eliminated
int width = 10;
int length = 20;

// 2. The math should fold: 10 * 20 = 200
int area = width * length;

// 3. Nested math folding
int height = 5;
int volume = area * height; // 200 * 5 = 1000

// 4. State preserving
// base_cost is modified later, so it will NOT be treated as a Clean Constant.
// It will be safely preserved.
int base_cost = 50;
int total_cost = base_cost + (volume / 10); // base_cost + 100

// 5. This modification blocks base_cost and total_cost from being propagated
total_cost++;
base_cost = 100;

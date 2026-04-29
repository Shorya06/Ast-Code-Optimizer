// Showcase: Strength Reduction, Relational Operators, and Control Flow

// 1. Bitwise Strength Reduction Test
// The optimizer should automatically convert *2 to <<1, and /2 to >>1
int size_limit = 100;
int optimized_mul = size_limit * 2;
int optimized_div = size_limit / 2;

// 2. Complex Relational Expression
int current_val = 50;
int threshold = 80;
int is_valid = (current_val >= threshold) || (size_limit != 0);

// 3. Pre and Post Increments
int loop_counter = 0;
loop_counter++;
--size_limit;

// 4. Branch Evaluation (Dead Branch Elimination)
// If the compiler evaluates an if(0) or mathematically impossible branch,
// it should completely strip the branch from the final code.
if (0) {
    int impossible_state = 999;
    impossible_state++;
}

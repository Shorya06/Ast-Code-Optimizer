# AST Code Optimizer

A compiler front-end for a C-like language, written in C with **Flex** and **Bison**. It lexes and parses source into an Abstract Syntax Tree (AST), applies multi-pass compiler optimizations, and regenerates optimized C code — with full AST visualization before and after.

## What it does

- **Lexer + parser** (Flex / Bison) for a C-like subset: `int`/`float`/`char`/`void` types, variable declarations and assignments, `if`/`else`, `while`, `for`, functions, arrays, `return`/`break`, `++`/`--`, and comparison/logical operators
- **AST construction** with printed tree output and Graphviz **DOT files** (`AST_before.dot` / `AST_after.dot`) for visualization
- **Optimization passes** (`optimizer.c`):
  - **Constant folding** — `2 + 3` → `5`, including `<<`, `>>`, and zero-division guards
  - **Constant propagation** — constant variables substituted at use sites, reassignment-aware
  - **Strength reduction** — `x * 2` → `x << 1`, `x / 2` → `x >> 1`
  - **Dead-code elimination** — unused constant declarations (side-effect checked), unreachable code after `return`, and `if (0)` branches
  - Optimization summary printed per pass
- **Code generation** — emits optimized C to the console and to `optimized_output.c`
- **`--lex` debug mode** — dumps the token stream without building the AST
- **Parser error recovery** with actionable hints (missing braces, invalid expressions, incomplete `for` loops, unexpected tokens)

## Tech stack

| Component | Tool |
|---|---|
| Language | C (GCC) |
| Lexer | Flex (`lexer.l`) |
| Parser | Bison (`parser.y`) |
| Build | Makefile / `build.bat` |

## Build & run

**Linux / macOS** (requires `gcc`, `flex`, `bison`):

```bash
make
./compiler.exe sample1_basic.c
```

**Windows** (requires `win_bison`, `win_flex`, `gcc`):

```bat
build.bat
```

### Usage

```bash
compiler.exe [--lex] <source_file>
```

Sample inputs: `sample1_basic.c` (data types & initialization), `sample2_expression.c` (expressions), `sample3_invalid.c` (error-recovery demo), `test_input.c`.

### Outputs

- AST dumps in the console, plus `ast_before.txt` / `ast_after.txt`
- Graphviz DOT files for visualization (`AST_before.dot`, `AST_after.dot`)
- Optimized code in `optimized_output.c`

## Project structure

```text
Ast-Code-Optimizer/
├── lexer.l        # Flex lexer
├── parser.y       # Bison grammar
├── ast.c / ast.h  # AST nodes, printing, DOT generation
├── optimizer.c / optimizer.h  # Multi-pass optimization engine
├── codegen.c / codegen.h      # Optimized C generation
├── main.c         # Driver (compile, optimize, visualize)
├── Makefile / build.bat
└── sample*.c      # Example inputs
```

## Author

**Shorya Tripathi** — [github.com/Shorya06](https://github.com/Shorya06)

Built for learning and educational purposes.

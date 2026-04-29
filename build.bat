@echo off
echo Generating parser with Bison...
win_bison -d parser.y
if %errorlevel% neq 0 (
    echo Bison failed!
    exit /b %errorlevel%
)

echo Generating lexer with Flex...
win_flex lexer.l
if %errorlevel% neq 0 (
    echo Flex failed!
    exit /b %errorlevel%
)

echo Compiling C code with GCC...
gcc -Wall -Wextra -g -o compiler.exe lex.yy.c parser.tab.c ast.c optimizer.c codegen.c main.c
if %errorlevel% neq 0 (
    echo GCC failed!
    exit /b %errorlevel%
)

echo Compilation successful! Executable: compiler.exe
echo.
echo ========================================
echo RUNNING SAMPLE 1 (Basic Output)
echo ========================================
compiler.exe sample1_basic.c
echo.
echo ========================================
echo RUNNING SAMPLE 2 (Expression Output)
echo ========================================
compiler.exe sample2_expression.c
echo.
echo ========================================
echo RUNNING SAMPLE 3 (Invalid Error Recovery)
echo ========================================
compiler.exe sample3_invalid.c
echo.
echo ========================================
echo RUNNING LEXER DEBUG MODE ON SAMPLE 2
echo ========================================
compiler.exe --lex sample2_expression.c
echo.
echo Complete!

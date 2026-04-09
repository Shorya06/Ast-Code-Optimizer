CC = gcc
LEX = flex
YACC = bison

CFLAGS = -Wall -Wextra -g
YACCFLAGS = -d

all: compiler

compiler: lex.yy.c parser.tab.c ast.c main.c
	$(CC) $(CFLAGS) -o compiler.exe lex.yy.c parser.tab.c ast.c main.c

lex.yy.c: lexer.l parser.tab.h
	$(LEX) lexer.l

parser.tab.c parser.tab.h: parser.y
	$(YACC) $(YACCFLAGS) parser.y

clean:
	rm -f lex.yy.c parser.tab.c parser.tab.h compiler.exe

CC=gcc
FLEX=flex
BISON=bison
splc: .lex .syntax
	@rm -rf bin/
	@mkdir bin
	$(CC) syntax.tab.c -lfl -ly -o bin/splc
	@chmod +x bin/splc
lexer: .lex
	$(CC) lex.yy.c -lfl -o lexer.out
.lex: lex.l
	$(FLEX) lex.l
.syntax: syntax.y
	$(BISON) -d syntax.y
clean:
	@rm -rf bin/
	@rm -f lex.yy.c syntax.tab.* *.out
.PHONY: splc
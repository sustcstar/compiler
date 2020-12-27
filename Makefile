PLUSPLUS=g++
FLEX=flex
BISON=bison
splc: .lex .syntax
	@rm -rf bin/
	@mkdir bin
	$(PLUSPLUS) parseTree.cpp syntax.tab.c semantic.cpp reportError.cpp ir.cpp -lfl -ly -g -o bin/splc 
	@chmod +x bin/splc
lexer: .lex
	$(PLUSPLUS) lex.yy.c -lfl -o lexer.out
# test: .lex .syntax
# 	$(PLUSPLUS) -Wall -Wextra -O -ansi -pedantic -shared -fPIC parseTree.cpp syntax.tab.c semantic.cpp -lfl -ly -g -o bin/splc.so
# -Wno-write-strings -Wno-int-to-pointer-cast
.lex: lex.l
	$(FLEX) lex.l
.syntax: syntax.y
	$(BISON) -t -d -v syntax.y
clean:
	@rm -rf bin/
	@rm -f lex.yy.c syntax.tab.* *.out
.PHONY: splc
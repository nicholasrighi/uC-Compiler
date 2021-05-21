GRAMMAR_FILE=grammar
PARSER_FILE=parser
EXE_NAME=compiler
BISON_FLAGS= -d --debug
CC=g++
C_FLAGS=-Wno-write-strings -std=c++17 -g

top: $(PARSER_FILE).l $(GRAMMAR_FILE).y
	bison $(BISON_FLAGS) $(GRAMMAR_FILE).y
	flex $(PARSER_FILE).l
	$(CC) $(C_FLAGS) $(wildcard *.cpp) grammar.tab.c lex.yy.c $(wildcard AST_classes/*.cpp) $(wildcard Supporting_classes/*.cpp) $(wildcard Visitor_classes/*.cpp) -lfl -o $(EXE_NAME)

clean:
	rm *.tab* $(EXE_NAME) *.yy.c* *.output*

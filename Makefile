GRAMMAR_FILE=grammar
PARSER_FILE=parser
EXE_NAME=compiler
BISON_FLAGS=-d 
CC=g++
C_FLAGS=-Wno-write-strings -std=c++17 -g

top: $(PARSER_FILE).l $(GRAMMAR_FILE).y
	bison $(BISON_FLAGS) $(GRAMMAR_FILE).y
	flex $(PARSER_FILE).l
	$(CC) $(C_FLAGS) -o $(EXE_NAME) grammar.tab.c Visitor_classes/Print_AST_visitor.cpp -lfl

clean:
	rm *.tab* $(EXE_NAME) *.yy.c* *.output*

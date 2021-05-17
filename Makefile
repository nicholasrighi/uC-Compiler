GRAMMAR_FILE=grammar
PARSER_FILE=parser
EXE_NAME=compiler
BISON_FLAGS=-d 
CC=g++
C_FLAGS=-Wno-write-strings -std=c++17 -g

top: $(PARSER_FILE).l $(GRAMMAR_FILE).y
	bison $(BISON_FLAGS) $(GRAMMAR_FILE).y
	flex $(PARSER_FILE).l
	$(CC) $(C_FLAGS) compiler.cpp grammar.tab.c lex.yy.c Visitor_classes/Print_AST_visitor.cpp Supporting_classes/Symbol_table.cpp Visitor_classes/Dec_before_use.cpp -lfl -o $(EXE_NAME)

clean:
	rm *.tab* $(EXE_NAME) *.yy.c* *.output*

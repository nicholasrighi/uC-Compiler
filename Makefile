GRAMMAR_FILE=grammar
PARSER_FILE=parser
EXE_NAME=compiler
BISON_FLAGS=-d

top: $(PARSER_FILE).l $(GRAMMAR_FILE).y
	bison $(BISON_FLAGS) $(GRAMMAR_FILE).y
	flex $(PARSER_FILE).l
	gcc -o $(EXE_NAME) $(GRAMMAR_FILE).tab.c -lfl

clean:
	rm *.tab* $(EXE_NAME) *.yy.c*
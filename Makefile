GRAMMAR_FILE=grammar
PARSER_FILE=parser
EXE_NAME=compiler
BISON_FLAGS= --debug -d
CC=g++
C_FLAGS=-Wall -Wpedantic -Wno-sign-compare -Wno-write-strings -std=c++17 -g 
SAN_FLAGS=-ggdb -fsanitize=undefined -fsanitize=address -fno-omit-frame-pointer -static-libstdc++ -static-libasan -lrt
TEST_DIR=known_good_test_files
TEST_FILE=dot_product.c
TEST_PATH=$(TEST_DIR)/$(TEST_FILE)

.PHONY:test

top: $(PARSER_FILE).l $(GRAMMAR_FILE).y
	bison $(BISON_FLAGS) $(GRAMMAR_FILE).y
	flex $(PARSER_FILE).l
	$(CC) $(C_FLAGS) $(wildcard *.cpp) grammar.tab.c lex.yy.c $(wildcard AST_classes/*.cpp) $(wildcard Supporting_classes/*.cpp) $(wildcard Visitor_classes/*.cpp) -lfl -o $(EXE_NAME)

debug: $(PARSER_FILE).l $(GRAMMAR_FILE).y
	bison $(BISON_FLAGS) $(GRAMMAR_FILE).y
	flex $(PARSER_FILE).l
	$(CC) $(C_FLAGS) $(SAN_FLAGS) $(wildcard *.cpp) grammar.tab.c lex.yy.c $(wildcard AST_classes/*.cpp) $(wildcard Supporting_classes/*.cpp) $(wildcard Visitor_classes/*.cpp) -lfl -o $(EXE_NAME)

test:
	./compiler -sf $(TEST_PATH) -o asm.s 
	gcc -g asm.s -o test
	gcc -O0 -Wall -g $(TEST_PATH) -o test2

clean:
	-rm *.tab* $(EXE_NAME) *.yy.c* *.output* test *.s debug_log.txt



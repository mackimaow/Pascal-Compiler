.PHONY: all clean  runTestYacc runTestLex runTestSymbolTable runTestTree runTestHashTable runTestLinkedList

ROUTINES = semantic_analyzer.o code_generator.o
YACC = y.tab.o
LEX = lex.yy.o
MISC = symboltable.o parsetree.o
COLLECTIONS = hashtable.o tree.o linkedlist.o iterator.o
UTILS = lexconstants.o utils.o object.o
GCC = -std=gnu99

all: pcc

# final script
pcc: pascal_compiler.c $(ROUTINES) $(YACC) $(LEX) $(MISC) $(COLLECTIONS) $(UTILS)
	gcc pascal_compiler.c $(GCC) $(ROUTINES) $(YACC) $(LEX) $(MISC) $(COLLECTIONS) $(UTILS) -o pcc -ll -ly -lm


# yacc
y.tab.o: y.tab.c tree.h $(LEX) $(MISC) $(COLLECTIONS) $(UITLS)
	gcc -c $(GCC) y.tab.c

y.tab.c: pascal.y  
	yacc -y -dv pascal.y


# lex
lex.yy.o: lex.yy.c  $(MISC) $(COLLECTIONS) $(UITLS)
	gcc -c $(GCC) lex.yy.c

lex.yy.c: pascal.l  
	lex -l pascal.l 



# Program Routines
semantic_analyzer.o: semantic_analyzer.h semantic_analyzer.c $(MISC) $(COLLECTIONS) $(UITLS)
	gcc -c $(GCC) semantic_analyzer.c

code_generator.o: code_generator.h code_generator.c  $(MISC) $(COLLECTIONS) $(UITLS)
	gcc -c $(GCC) code_generator.c


# Misc

symboltable.o: symboltable.h symboltable.c $(COLLECTIONS) $(UITLS)
	gcc -c $(GCC) symboltable.c

parsetree.o: parsetree.h parsetree.c $(COLLECTIONS) $(UITLS)
	gcc -c $(GCC) parsetree.c

# Generic custom collections

hashtable.o: hashtable.h hashtable.c linkedlist.o iterator.o $(UITLS)
	gcc -c $(GCC) hashtable.c

tree.o: tree.h tree.c linkedlist.o iterator.o $(UITLS)
	gcc -c $(GCC) tree.c

linkedlist.o: linkedlist.h linkedlist.c iterator.o $(UITLS)
	gcc -c $(GCC) linkedlist.c

iterator.o: iterator.c iterator.h
	gcc -c $(GCC) iterator.c


# Utility
lexconstants.o: lexconstants.h lexconstants.c
	gcc -c $(GCC) lexconstants.c 

utils.o: utils.c utils.h
	gcc -c $(GCC) utils.c

object.o: object.c object.h
	gcc -c $(GCC) object.c


# run tests
runTestYacc: testYacc
	@echo ""
	@echo " Testing Yacc "
	@echo ""
	@./testYacc

runTestLex: testLex
	@echo ""
	@echo " Testing Lex "
	@echo ""
	@./testLex

runTestSymbolTable: testSymbolTable
	@echo ""
	@echo " Testing Symbol Table "
	@echo ""
	@./testSymbolTable

runTestTree: testTree
	@echo ""
	@echo " Testing Tree "
	@echo ""
	@./testTree

runTestHashTable: testHashTable
	@echo ""
	@echo " Testing Hash Table "
	@echo ""
	@./testHashTable

runTestLinkedList: testLinkedList
	@echo ""
	@echo " Testing Linked List "
	@echo ""
	@./testLinkedList


# test executables
testYacc: testYacc.c $(YACC) $(LEX) $(MISC) $(COLLECTIONS) $(UTILS)
	gcc testYacc.c  $(GCC) $(YACC) $(LEX) $(MISC) $(COLLECTIONS) $(UTILS) -o testYacc -ll -ly  -lm

testLex: testLex.c symboltable.o $(LEX) $(COLLECTIONS) $(UTILS)
	gcc testLex.c symboltable.o $(GCC) $(LEX) $(COLLECTIONS) $(UTILS) -o testLex -ll  -lm

testSymbolTable: testSymbolTable.c $(MISC) $(COLLECTIONS) $(UTILS)
	gcc testSymbolTable.c $(GCC) $(MISC) $(COLLECTIONS) $(UTILS) -o testSymbolTable  -lm

testHashTable: testHashTable.c $(COLLECTIONS) $(UTILS)
	gcc testHashTable.c $(GCC) $(COLLECTIONS) $(UTILS) -o testHashTable  -lm

testTree: testTree.c $(COLLECTIONS) $(UTILS)
	gcc testTree.c $(GCC) $(COLLECTIONS) $(UTILS) -o testTree  -lm

testLinkedList: testLinkedList.c $(COLLECTIONS) $(UTILS)
	gcc testLinkedList.c $(GCC) $(COLLECTIONS) $(UTILS) -o testLinkedList  -lm


# clean
clean:
	rm -f *.o *.gch pcc *.out y.tab.* lex.yy.* *.output *.asm
	rm -f testHashTable testLinkedList testSymbolTable testTree testLex testYacc
	

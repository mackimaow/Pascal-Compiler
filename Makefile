.PHONY: all clean  runTestYacc runTestLex runTestSymbolTable runTestTree runTestHashTable runTestLinkedList

ROUTINES = semantic_analyzer
YACC = y.tab.o
LEX = lex.yy.o
MISC = symboltable.o parsetree.o
COLLECTIONS = hashtable.o tree.o linkedlist.o
UTILS = lexconstants.o utils.o object.o

all: pcc


pcc: pascal_compiler.c $(ROUTINES) $(YACC) $(LEX) $(MISC) $(COLLECTIONS) $(UITLS)
	gcc pascal_compiler $(ROUTINES) $(YACC) $(LEX) $(MISC) $(COLLECTIONS) $(UITLS) -o pcc -ll -ly


# yacc
y.tab.o: y.tab.c tree.h $(LEX) $(MISC) $(COLLECTIONS) $(UITLS)
	gcc -c y.tab.c

y.tab.c: pascal.y  
	yacc -y -dv pascal.y


# lex
lex.yy.o: lex.yy.c  $(MISC) $(COLLECTIONS) $(UITLS)
	gcc -c lex.yy.c

lex.yy.c: pascal.l  
	lex -l pascal.l 



# Program Routines
semantic_analyzer.o: semantic_analyzer.h semantic_analyzer.c
	gcc -c semantic_analyzer.c

code_generator.o: code_generator.h code_generator.c 
	gcc -c code_generator.c


# Misc

symboltable.o: symboltable.h symboltable.c $(COLLECTIONS) $(UITLS)
	gcc -c symboltable.c

parsetree.o: parsetree.h parsetree.c $(COLLECTIONS) $(UITLS)
	gcc -c parsetree.c

# generic custom collections

hashtable.o: hashtable.h hashtable.c linkedlist.o $(UITLS)
	gcc -c hashtable.c

tree.o: tree.h tree.c linkedlist.o $(UITLS)
	gcc -c tree.c

linkedlist.o: linkedlist.h linkedlist.c $(UITLS)
	gcc -c linkedlist.c 


# Utility
lexconstants.o: lexconstants.h lexconstants.c
	gcc -c lexconstants.c 

utils.o: utils.c utils.h
	gcc -c utils.c

object.o: object.c object.h
	gcc -c object.c


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
	gcc testYacc.c $(YACC) $(LEX) $(MISC) $(COLLECTIONS) $(UTILS) -o testYacc -ll -ly

testLex: testLex.c symboltable.o $(LEX) $(COLLECTIONS) $(UTILS)
	gcc testLex.c symboltable.o $(LEX) $(COLLECTIONS) $(UTILS) -o testLex -ll

testSymbolTable: testSymbolTable.c $(MISC) $(COLLECTIONS) $(UTILS)
	gcc testSymbolTable.c $(MISC) $(COLLECTIONS) $(UTILS) -o testSymbolTable

testHashTable: testHashTable.c $(COLLECTIONS) $(UTILS)
	gcc testHashTable.c $(COLLECTIONS) $(UTILS) -o testHashTable

testTree: testTree.c $(COLLECTIONS) $(UTILS)
	gcc testTree.c $(COLLECTIONS) $(UTILS) -o testTree

testLinkedList: testLinkedList.c $(COLLECTIONS) $(UTILS)
	gcc testLinkedList.c $(COLLECTIONS) $(UTILS) -o testLinkedList


# clean
clean:
	rm -f *.o *.gch pcc *.out y.tab.* lex.yy.* *.output
	rm -f testHashTable testLinkedList testSymbolTable testTree testLex testYacc
	

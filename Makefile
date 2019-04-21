.PHONY: all clean runTestHashTable runTestLinkedList

all: pcc


pcc: pascal_compiler.c hashtable.o linkedlist.o
	gcc pascal_compiler -o pcc

hashtable.o: hashtable.h hashtable.c
	gcc -c hashtable.c

linkedlist.o: linkedlist.h linkedlist.c
	gcc -c linkedlist.c 

utils.o: utils.c utils.h
	gcc -c utils.c

object.o: object.c object.h
	gcc -c object.c


# test files
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

testHashTable: testHashTable.c hashtable.o linkedlist.o utils.o object.o
	gcc testHashTable.c hashtable.o linkedlist.o utils.o object.o -o testHashTable

testLinkedList: testLinkedList.c linkedlist.o utils.o object.o
	gcc testLinkedList.c linkedlist.o utils.o object.o -o testLinkedList


# clean
clean:
	rm -f *.o *.gch pcc *.out 
	rm -f testHashTable testLinkedList
	

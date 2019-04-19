
all: pcc

test: test.c
	gcc test.c -o test

pcc: pascal_compiler.c hashtable.o linkedlist.o
	gcc pascal_compiler -o pcc

hashtable.o: hashtable.h
	gcc -c hashtable.h

linkedlist.o: linkedlist.h
	gcc -c linkedlist.h	

clean:
	rm -f *.o pcc test
	

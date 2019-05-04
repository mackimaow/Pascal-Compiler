#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "tree.h"


char * toCharString(ObjectType * type, void* element) {
	char * stringValue = (char *) malloc(sizeof(char)*2);
	stringValue[0] = *(char *)element;
	stringValue[1] = '\0';
	return stringValue;
}

void deconstElem(ObjectType * type, void* element) {
	// free(element);
}

static ObjectType * CHAR;




int main() {
	CHAR = objectTypeInit(toCharString, 0, deconstElem);
	char * values = "abcdefghijklmnopqrstuvwxyz";
	
	Tree * tree = treeInit(CHAR, values);
	treePrint(tree);
	printf("\n\n");

	treeAddLeafs(tree, 3, values+1, values+2, values+3);
	treePrint(tree);
	printf("\n\n");

	Tree * subTree = treeGetChild(tree, 0);
	treeAddLeafs(subTree, 3, values+4, values+5, values+6);
	treePrint(tree);
	printf("\n\n");

	Tree * subSubTree = treeGetChild(subTree, 2);
	treeAddLeafs(subSubTree, 3, values+7, values+8, values+9);
	treePrint(tree);
	printf("\n\n");

	subSubTree = treeGetChild(subTree, 1);
	treeAddLeafs(subSubTree, 3, values+10, values+11, values+12);
	treePrint(tree);
	printf("\n\n");

	treeAddLeafs(tree, 3, values+13, values+14, values+15);
	treePrint(tree);
	printf("\n\n");

	treePrint(subTree);
	printf("\n\n");

	treeDestroy(subTree);
	printf("\n\n");

	treePrint(tree);
	printf("\n\n");
}
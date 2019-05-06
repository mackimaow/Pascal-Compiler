#include <stdarg.h>
#include "stdio.h"
#include "stdlib.h"
#include "tree.h"
#include "linkedlist.h"
#include "utils.h"

struct Tree {
	Tree * parent;
	ObjectType * treeObjectType;
	void * element;
	LinkedList * children;	
};


// tree object type (only used in printing linkedLists) 
static char * treeTypeToString(ObjectType * objectType, void * value) {
	char * treeString = treeToString((Tree *) value);
	return treeString;
}

static void treeTypeDestroy(ObjectType * objectType, void * value) {
	Tree * tree = (Tree *) value; 
	ObjectType * elementType = objectTypeGetNestedType(tree->treeObjectType, 0);
	objectTypeDestroyValue(elementType, tree->element);
	objectTypeDestroyTypeOnly(tree->treeObjectType);
	linkedListDestroy(tree->children);
}



// 	util functions

static char * treeString ( char * stringBehind, Tree * parentTree );

static bool doRecursiveToString(ObjectType * elementType, int index, void* element, void** parameters, ForEachOptions* options) {
	Tree * tree = (Tree *) element;
	Single * single = (Single *) parameters[0];
	LinkedList * stringList = (LinkedList *) parameters[1];
	char * middleHorizonalString = (char *) parameters[2];
	char * lastHorizontalString = (char *) parameters[3];

	int * currentIndex = (int *)single->element;
	if(*currentIndex == 0)
		linkedListPushBack(stringList, treeString ( lastHorizontalString, tree));
	else
		linkedListPushBack(stringList, treeString ( middleHorizonalString, tree));
	*currentIndex = *currentIndex - 1;
	return false;
}




// implementated functions

Tree * treeInit ( ObjectType * elementType, void* element) {
	return treeInitWithPrintProperties ( elementType, element, &defaultTreePrintProperties);
}

Tree * treeInitWithPrintProperties ( ObjectType * elementType, void* element, TreePrintProperties * printProperties) {
	ObjectType * treeObjectType = objectTypePropertiesNestedInit(treeTypeToString, 0, treeTypeDestroy, 1, 1, elementType, printProperties);
	Tree * tree = (Tree *) malloc(sizeof(Tree));
	tree->treeObjectType = treeObjectType;
	tree->element = element;
	tree->children = linkedListInit(treeObjectType);
	tree->parent = 0;
	return tree;
}

void treeDestroy ( Tree * tree ) {
	if(tree->parent) {
		LinkedList * linkedList = tree->parent->children;
		bool stoppedShort = linkedListRemoveElement(linkedList, tree);
		if(stoppedShort)
			return;
	}
	ObjectType * elementType = objectTypeGetNestedType(tree->treeObjectType, 0);
	objectTypeDestroyValue(elementType, tree->element);
	objectTypeDestroyTypeOnly(tree->treeObjectType);
	linkedListDestroy(tree->children);
}

void * treeGetElement( Tree * tree ) {
	return tree->element;
}

void treeAddNodes(Tree * parentTree, int numChildren, ...) {
	if(numChildren > 0) {
		va_list   argList;
		va_start( argList, numChildren);
		for( int i = 0; i < numChildren; i++ ) {
			Tree * childTree = (Tree *) va_arg( argList, Tree * );
			treeAddNode(parentTree, childTree);
		}
  		va_end( argList );
  	}
}


void treeAddNode(Tree * parentTree, Tree * childTree) {
	childTree->parent = parentTree;
	linkedListPushBack(parentTree->children, childTree);
}

void treeAddLeafs( Tree * parentTree, int numChildren, ... ) {
	ObjectType * elementType = objectTypeGetNestedType(parentTree->treeObjectType, 0);
	TreePrintProperties * printProperties = (TreePrintProperties *) objectTypeGetProperty(parentTree->treeObjectType, 0);
	if(numChildren > 0) {
		va_list   argList;
		va_start( argList, numChildren);
		for( int i = 0; i < numChildren; i++ )
			treeAddLeaf( parentTree, va_arg( argList, void * ) );
  		va_end( argList );
  	}
}

void treeAddLeaf( Tree * parentTree, void * element ) {
	ObjectType * elementType = objectTypeGetNestedType(parentTree->treeObjectType, 0);
	TreePrintProperties * printProperties = (TreePrintProperties *) objectTypeGetProperty(parentTree->treeObjectType, 0);
	Tree * childTree = treeInitWithPrintProperties(elementType, element, printProperties);
	childTree->parent = parentTree;
	linkedListPushBack(parentTree->children, childTree);
}

bool treeIsLeaf( Tree * parentTree ) {
	return linkedListGetSize(parentTree->children) == 0;
}

LinkedList * treeGetChildren(Tree * parentTree) {
	return parentTree->children;
}

Tree * treeGetChild(Tree * parentTree, int i) {
	return (Tree *) linkedListGet(parentTree->children, i);
}

Tree * treeGetParent(Tree * tree) {
	return tree->parent;
}

int  treeGetSize( Tree * parentTree ) {
	return linkedListGetSize(parentTree->children);
}

void treePrint( Tree * parentTree ) {
	char* stringValue = treeToString( parentTree );
	printf("%s", stringValue);
	free(stringValue);
}


char * treeToString( Tree * parentTree ) {
	return treeString("", parentTree);
}





static char * treeString ( char * stringBehind, Tree * parentTree ) {
	ObjectType * elementType = objectTypeGetNestedType(parentTree->treeObjectType, 0);
	TreePrintProperties * printProperties = (TreePrintProperties *) objectTypeGetProperty(parentTree->treeObjectType, 0);

	char * left = printProperties->left;
	char * right = printProperties->right;
	int leftLength = getStringSize(left);
	int rightLength = getStringSize(right);
	char * treeElementString = objectTypeToString(elementType, parentTree->element);
	int treeElementStringLength = getStringSize(treeElementString);
	
	if(treeIsLeaf(parentTree)) { // tree is leaf
		int totalLength = leftLength + rightLength + treeElementStringLength + 1;
		char * temp = malloc(sizeof(char) * (totalLength + 1));
		int currentIndex = 0;
		stringInsert(temp, left, currentIndex);
		currentIndex += leftLength;
		stringInsert(temp, treeElementString, currentIndex);
		currentIndex += treeElementStringLength;
		stringInsert(temp, right, currentIndex);

		*(temp+totalLength-1) = '\n';
		*(temp+totalLength) = '\0';
		free(treeElementString);
		return temp;
	}

	char * horizontal = printProperties->horizontal;
	char * vertical   = printProperties->vertical;
	int horizontalSize = getStringSize(horizontal);
	int verticalSize = getStringSize(vertical);
	int stringBehindLength = getStringSize(stringBehind);
	int nextStringBehindSize = stringBehindLength + verticalSize + horizontalSize;


	char * firstHorizontalString = malloc(sizeof(char) * (nextStringBehindSize + 1));
	char * middleHorizonalString = malloc(sizeof(char) * (nextStringBehindSize + 1));
	char * lastHorizontalString = malloc(sizeof(char) * (nextStringBehindSize + 1));

	stringInsert(firstHorizontalString, stringBehind, 0);
	stringInsert(firstHorizontalString, vertical, stringBehindLength);
	stringInsert(firstHorizontalString, horizontal, stringBehindLength+verticalSize);
	firstHorizontalString[nextStringBehindSize] = '\0';

	stringInsert(middleHorizonalString, stringBehind, 0);
	stringInsert(middleHorizonalString, vertical, stringBehindLength);
	for(int i = stringBehindLength+verticalSize; i < nextStringBehindSize; i++)
		middleHorizonalString[i]=' ';
	middleHorizonalString[nextStringBehindSize] = '\0';

	stringInsert(lastHorizontalString, stringBehind, 0);
	for(int i = stringBehindLength; i < nextStringBehindSize; i++)
		lastHorizontalString[i]=' ';
	lastHorizontalString[nextStringBehindSize] = '\0';

	ListPrintProperties printProperties2 = {firstHorizontalString, firstHorizontalString, ""};
	LinkedList * stringList = linkedListInitWithPrintProperties(&STRING_OBJECT, &printProperties2);
	int size = treeGetSize( parentTree ) - 1;
	Single single;
	single.element = &size;
	linkedListForEach(parentTree->children, doRecursiveToString, 4, &single, stringList, middleHorizonalString, lastHorizontalString);
	
	char * stringAfter = linkedListToString(stringList);
	int stringAfterSize = getStringSize(stringAfter);
	int totalSize = leftLength + treeElementStringLength + rightLength + 1 + stringAfterSize;
	char * temp = malloc(sizeof(char) * (totalSize + 1));
	int currentIndex = 0;
	stringInsert(temp, left, currentIndex);
	currentIndex += leftLength;
	stringInsert(temp, treeElementString, currentIndex);
	currentIndex += treeElementStringLength;
	stringInsert(temp, right, currentIndex);
	currentIndex += rightLength;
	temp[currentIndex] = '\n';
	currentIndex++;
	stringInsert(temp, stringAfter, currentIndex);
	temp[totalSize] = '\0';
	free(stringAfter);
	linkedListDestroy(stringList);
	free(lastHorizontalString);
	free(middleHorizonalString);
	free(firstHorizontalString);
	free(treeElementString);
	return temp;
}
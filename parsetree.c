#include <stdarg.h>
#include "parsetree.h"
#include "utils.h"
#include "lexconstants.h"

struct TreeValue {
	int type;
	int labelValue;
	char * attrValue;
	int lineNumberDeclared;
	int lineIndexDeclared;
};

const bool TREE_NODE = true;
const bool TREE_LEAF = false;

const int NO_TYPE = -1;
const int NULL_TYPE = -2; 


static char * treeValueToString(ObjectType * objectType, void * value) {
	TreeValue * treeValue = (TreeValue * ) value;
	char * lexType = lexConstantToString(treeValue->type);
	char * attrString = treeValue->attrValue;
	char * barrier;
	int attrStringSize = 0;
	int labelValue = treeValue->labelValue;
	char * labelString;
	if(labelValue >= 0) {
		char * frontString = " (";
		char * middleString = intToString(labelValue);
		char * backString = ") ";
		int frontStringLength = getStringSize(frontString);
		int middleStringLength = getStringSize(middleString);
		int backStringLength = getStringSize(backString);
		int totalSize = frontStringLength + middleStringLength + backStringLength;
		labelString = malloc(sizeof(char) * (totalSize + 1));
		stringInsert(labelString, frontString, 0);
		stringInsert(labelString, middleString, frontStringLength);
		stringInsert(labelString, backString, frontStringLength+middleStringLength);
		labelString[totalSize] = '\0';
		free(middleString);
	} else{
		labelString = copyString("");
	}
	int labelSize = getStringSize(labelString); 

	if(!attrString || treeValue->type < 0 ) {
		barrier = "";
	} else {
		barrier = "|";
	}
	if(attrString){
		attrStringSize = getStringSize(attrString);
	} else{
		attrString = "";
	}

	int barrierSize = getStringSize(barrier); 
	int lexTypeSize = getStringSize(lexType);

	int totalSize = lexTypeSize + barrierSize + attrStringSize + labelSize;
	char * temp = malloc(sizeof(char) * (totalSize + 1));
	
	stringInsert(temp, lexType, 0);
	stringInsert(temp, barrier, lexTypeSize);
	stringInsert(temp, attrString, lexTypeSize + barrierSize);
	stringInsert(temp, labelString, lexTypeSize + barrierSize + attrStringSize);
	temp[totalSize] = '\0';
	free(labelString);
	free(lexType);
	return temp;
}


static ObjectType * TREE_VALUE_TYPE = 0;

static ObjectType * getTreeValueType() {
	if(!TREE_VALUE_TYPE)
		TREE_VALUE_TYPE = objectTypeInit(treeValueToString, 0, 0);
	return TREE_VALUE_TYPE;
}

TreeValue * parseTreeInitTreeValue(int type, char * attrValue, int lineNumberDeclared, int lineIndexDeclared) {
	TreeValue * treeValue = malloc(sizeof(TreeValue *));
	treeValue->type = type;
	treeValue->labelValue = -1;
	if(attrValue)
		treeValue->attrValue = copyString(attrValue);
	else
		treeValue->attrValue = 0;
	treeValue->lineNumberDeclared =lineNumberDeclared;
	treeValue->lineIndexDeclared =lineIndexDeclared;
}


Tree * parseTreeInit( TreeValue * treeValue, int numberElements, ...) {
	Tree * tree = treeInit(getTreeValueType(), treeValue);

	if(numberElements != 0) {
		va_list   argList;
		va_start( argList, numberElements);
		for(int i = 0; i < numberElements; i++) {
			bool type = va_arg( argList, int );
			if(type == TREE_NODE)
				treeAddNode(tree, va_arg( argList, Tree * ));
			else
				treeAddLeaf(tree, va_arg( argList, TreeValue * ));
		}
	  	va_end( argList );
  	}
  	return tree;
}


Tree * parseTreeInitNew( char * stringValue , TreeValue * reference, int numberElements, ...)  {
	int lineNumberDeclared = -1;
	int lineIndexDeclared = -1;
	if(reference) {
		lineNumberDeclared = reference->lineNumberDeclared;
		lineIndexDeclared = reference->lineIndexDeclared;
	}
	TreeValue * newValue = parseTreeInitTreeValue(NO_TYPE, stringValue, lineNumberDeclared , lineIndexDeclared);

	Tree * tree = treeInit(getTreeValueType(), newValue);

	if(numberElements != 0) {
		va_list   argList;
		va_start( argList, numberElements);
		for(int i = 0; i < numberElements; i++) {
			bool type = va_arg( argList, int );
			if(type == TREE_NODE)
				treeAddNode(tree, va_arg( argList, Tree * ));
			else
				treeAddLeaf(tree, va_arg( argList, TreeValue * ));
		}
	  	va_end( argList );
  	}
  	return tree;
}


Tree * parseTreeInitCombine( TreeValue * treeValueType, TreeValue * treeValueAttrValue, int numberElements, ...)  {
	int lineNumberDeclared = treeValueType->lineNumberDeclared;
	int lineIndexDeclared = treeValueType->lineIndexDeclared;
	char * attributeValue = treeValueAttrValue->attrValue;
	TreeValue * newValue = parseTreeInitTreeValue(treeValueType->type, attributeValue, lineNumberDeclared, lineIndexDeclared);
	Tree * tree = treeInit(getTreeValueType(), newValue);


	if(numberElements != 0) {
		va_list   argList;
		va_start( argList, numberElements);
		for(int i = 0; i < numberElements; i++) {
			bool type = va_arg( argList, int );
			if(type == TREE_NODE)
				treeAddNode(tree, va_arg( argList, Tree * ));
			else
				treeAddLeaf(tree, va_arg( argList, TreeValue * ));
		}
	  	va_end( argList );
  	}
  	return tree;
}

static Tree * getSafeTree(char * stringValue , TreeValue * reference, Tree * parentTree) {
	Tree * tree = parentTree;
	if(!parentTree || parseTreeGetType(parentTree) == NULL_TYPE) {
		int lineNumberDeclared = -1;
		int lineIndexDeclared = -1;
		if(reference) {
			lineNumberDeclared = reference->lineNumberDeclared;
			lineIndexDeclared = reference->lineIndexDeclared;
		}
		TreeValue * newValue = parseTreeInitTreeValue(NO_TYPE, stringValue, lineNumberDeclared, lineIndexDeclared);
		tree = parseTreeInit(newValue, 0);
	}
	return tree;
}

Tree * parseTreeSafelyAddNodes(char * stringValue , TreeValue * reference, Tree * parentTree, int numberElements, ...) {
	Tree * tree = getSafeTree(stringValue , reference, parentTree);
	if(numberElements != 0) {
		va_list   argList;
		va_start( argList, numberElements);
		for(int i = 0; i < numberElements; i++)
			treeAddNode(tree, va_arg( argList, Tree * ));
	  	va_end( argList );
  	}
	return tree;
}

Tree * parseTreeSafelyAddNode(char * stringValue , TreeValue * reference, Tree * parentTree, Tree * childTree) {
	Tree * tree = getSafeTree(stringValue , reference, parentTree);
	treeAddNode(tree, childTree);
	return tree;
}

Tree * parseTreeSafelyAddLeafs(char * stringValue , TreeValue * reference, Tree * parentTree, int numberElements, ... ) {
	Tree * tree = getSafeTree(stringValue , reference, parentTree);
	if(numberElements != 0) {
		va_list   argList;
		va_start( argList, numberElements);
		for(int i = 0; i < numberElements; i++)
			treeAddLeaf(tree, va_arg( argList, TreeValue * ));
	  	va_end( argList );
  	}
	return tree;
}

Tree * parseTreeSafelyAddLeaf(char * stringValue , TreeValue * reference, Tree * parentTree, void * element ) {
	Tree * tree = getSafeTree(stringValue , reference, parentTree);
	treeAddLeaf(tree, element);
	return tree;
}


void parseTreeSetLabel(Tree * tree, int labelValue) {
	TreeValue * treeValue = (TreeValue *) treeGetElement(tree);
	treeValue->labelValue = labelValue;
}


int parseTreeGetLabel(Tree * tree) {
	TreeValue * treeValue = (TreeValue *) treeGetElement(tree);
	return treeValue->labelValue;
}


void parseTreeSetType(Tree * tree, int type) {
	TreeValue * treeValue = (TreeValue *) treeGetElement(tree);
	treeValue->type = type;
}


int parseTreeGetType(Tree * tree) {
	TreeValue * treeValue = (TreeValue *) treeGetElement(tree);
	return treeValue->type;
}

char * parseTreeGetAttribute(Tree * tree) {
	TreeValue * treeValue = (TreeValue *) treeGetElement(tree);
	return treeValue->attrValue;
}

int parseTreeGetLineNumberDeclared(Tree * tree) {
	TreeValue * treeValue = (TreeValue *) treeGetElement(tree);
	return treeValue->lineNumberDeclared;
}

int parseTreeGetLineIndexDeclared(Tree * tree) {
	TreeValue * treeValue = (TreeValue *) treeGetElement(tree);
	return treeValue->lineIndexDeclared;
} 

Tree * parseTreeGetNullNode(char * stringToPrint) {
	TreeValue * temp = parseTreeInitTreeValue(NULL_TYPE, stringToPrint, 0, 0); 
	Tree * NULL_TREE_NODE = parseTreeInit(temp, 0);
	return NULL_TREE_NODE;
}

bool parseTreeIsNull(Tree * tree) {
	int type = parseTreeGetType(tree);
	return type == NULL_TYPE;
}
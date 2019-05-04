#ifndef PARSE_TREE_H
#define PARSE_TREE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "tree.h"

typedef struct TreeValue TreeValue;

extern const bool TREE_NODE;
extern const bool TREE_LEAF;

const int NO_TYPE;
const int NULL_TYPE;

TreeValue * parseTreeInitTreeValue(int type, char * attrValue, int lineNumberDeclared, int lineIndexDeclared);

Tree * parseTreeInit( TreeValue * treeValue, int numberElements, ...) ;
Tree * parseTreeInitNew( char * stringValue , TreeValue * reference, int numberElements, ...) ;
Tree * parseTreeInitCombine( TreeValue * treeValueType, TreeValue * treeValueAttrValue, int numberElements, ...) ;

Tree * parseTreeSafelyAddNodes(char * stringValue , TreeValue * reference, Tree * parentTree, int numberElements, ...);
Tree * parseTreeSafelyAddNode(char * stringValue , TreeValue * reference, Tree * parentTree, Tree * childTree);
Tree * parseTreeSafelyAddLeafs(char * stringValue , TreeValue * reference, Tree * parentTree, int numberElements, ... );
Tree * parseTreeSafelyAddLeaf(char * stringValue , TreeValue * reference, Tree * parentTree, void * element );

int parseTreeGetType(Tree * tree);
char * parseTreeGetAttribute(Tree * tree);
int parseTreeGetLineNumberDeclared(Tree * tree);
int parseTreeGetLineIndexDeclared(Tree * tree);

Tree * parseTreeGetNullNode();

#endif // PARSE_TREE_H



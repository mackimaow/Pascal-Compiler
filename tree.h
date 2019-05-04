#ifndef TREE_H
#define TREE_H

#include <stdbool.h>
#include "utils.h"
#include "linkedlist.h"
#include "object.h"


typedef struct Tree Tree;


Tree * treeInit ( ObjectType * elementType, void* element );
Tree * treeInitWithPrintProperties ( ObjectType * elementType, void* element, TreePrintProperties * printProperties);
void treeDestroy ( Tree * parentTree );
void * treeGetElement( Tree * parentTree );
void treeAddNodes(Tree * parentTree, int numChildren, ...);
void treeAddNode(Tree * parentTree, Tree * childTree);
void treeAddLeafs( Tree * parentTree, int numChildren, ... );
void treeAddLeaf( Tree * parentTree, void * element );
bool treeIsLeaf( Tree * parentTree );
LinkedList * treeGetChildren(Tree * parentTree);
Tree * treeGetChild(Tree * parentTree, int i);
Tree * treeGetParent(Tree * tree);
int  treeGetSize( Tree * parentTree );
void treePrint( Tree * parentTree );
char * treeToString( Tree * parentTree );



#endif // TREE_H
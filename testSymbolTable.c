#include <string.h>
#include <stdio.h>
#include "symboltable.h"


int main () {
	SymbolTable * table = symbolTableInit();
	symbolTablePrint(table);
	printf("\n\n\n");



	symbolTablePushScope(table, "Main");
	symbolTablePrint(table);
	printf("\n\n\n");

	VariableAttr attr = {"Function"};
	VariableAttr attr1 = {"Variable"};
	symbolTablePut(table, "main", &attr);
	symbolTablePrint(table);
	printf("\n\n\n");

	symbolTablePut(table, "foo", &attr);
	symbolTablePrint(table);
	printf("\n\n\n");

	symbolTablePut(table, "sum", &attr1);
	symbolTablePrint(table);
	printf("\n\n\n");



	symbolTablePushScope(table, "SubProgram");
	symbolTablePrint(table);
	printf("\n\n\n");

	symbolTablePut(table, "TempProg", &attr);
	symbolTablePrint(table);
	printf("\n\n\n");

	symbolTablePut(table, "OtherProg", &attr);
	symbolTablePrint(table);
	printf("\n\n\n");

	symbolTablePut(table, "a", &attr1);
	symbolTablePrint(table);
	printf("\n\n\n");

	ListPrintProperties listPrint = {"[", ".", "]"};
	char * scopeString = symbolTableScopeTraceString(table, &listPrint);
	printf("Scope String: %s\n", scopeString);
	printf("\n\n\n");



	symbolTablePushScope(table, "SubSubProgram");
	symbolTablePrint(table);
	printf("\n\n\n");

	scopeString = symbolTableScopeTraceString(table, &listPrint);
	printf("Scope String: %s\n", scopeString);
	printf("\n\n\n");


	symbolTablePopScope(table);
	symbolTablePrint(table);
	printf("\n\n\n");



	symbolTablePopScope(table);
	symbolTablePrint(table);
	printf("\n\n\n");
}
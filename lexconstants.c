#include "stdio.h"
#include "stdlib.h"
#include "lexconstants.h"
#include "utils.h"

int lineNumber = 1;
int lineIndex = 1;


void processLexString(char * string) {
	char c = string[0];
	int i = 0;
	while(c != '\0') {
		if(c == '\n') {
			lineIndex = 1;
			lineNumber++;			
		} else {
			lineIndex++;
		}
		i++;
		c = string[i];
	}
}

void lexError(char * stringValue) {
	fprintf(stderr,"[ERROR] LEXICAL ANALYZER FAILED AT (%i:%i) -> %s?\n", lineNumber, lineIndex, stringValue);
	exit(1);
}


char * lexConstantToString(int value) {
	switch(value) {
		case LL_BOOLEAN: return copyString("L_BOOLEAN");
		case LL_PROGRAM: return copyString("L_PROGRAM");
		case LL_FUNCTION: return copyString("L_FUNCTION");
		case LL_PROCEDURE: return copyString("L_PROCEDURE");
		case LL_IF: return copyString("L_IF");
		case LL_THEN: return copyString("L_THEN");
		case LL_ELSE: return copyString("L_ELSE");
		case LL_WHILE: return copyString("L_WHILE");
		case LL_FOR: return copyString("L_FOR");
		case LL_TO: return copyString("L_TO");
		case LL_DO: return copyString("L_DO");
		case LL_BEGIN: return copyString("L_BEGIN");
		case LL_END: return copyString("L_END");
		case LL_INTEGER: return copyString("L_INTEGER");
		case LL_REAL: return copyString("L_REAL");
		case LL_VAR: return copyString("L_VAR");
		case LL_ARRAY: return copyString("L_ARRAY");
		case LL_OF: return copyString("L_OF");
		case LL_NOT: return copyString("L_NOT");
		case LL_ADD: return copyString("L_ADD");
		case LL_SUB: return copyString("L_SUB");
		case LL_OR: return copyString("L_OR");
		case LL_MULOP: return copyString("L_MULOP");
		case LL_RELOP: return copyString("L_RELOP");
		case LL_ASSIGNOP: return copyString("L_ASSIGNOP");
		case LL_ID: return copyString("L_ID");
		case LL_NUM: return copyString("L_NUM");
		case LL_LP: return copyString("L_LP");
		case LL_RP: return copyString("L_RP");
		case LL_LB: return copyString("L_LB");
		case LL_RB: return copyString("L_RB");
		case LL_DD: return copyString("L_DD");
		case LL_D: return copyString("L_D");
		case LL_COM: return copyString("L_COM");
		case LL_SC: return copyString("L_SC");
		case LL_C: return copyString("L_C");
		default:
		return copyString("");
	}
}

char * lexConstantToStringNoLL(int value) {
	char * temp = lexConstantToString(value);
	char * noLL = stringTakeLast(temp, 2);
	free(temp);
	return noLL;
}
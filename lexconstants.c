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
	fprintf(stderr, "LEXICAL ERROR AT %i:%i -> %s?\n", lineNumber, lineIndex, stringValue);
	exit(1);
}

char * lexConstantToString(int value) {
	switch(value) {
		case 258: return copyString("L_PROGRAM");
		case 259: return copyString("L_FUNCTION");
		case 260: return copyString("L_PROCEDURE");
		case 261: return copyString("L_IF");
		case 262: return copyString("L_THEN");
		case 263: return copyString("L_ELSE");
		case 264: return copyString("L_WHILE");
		case 265: return copyString("L_FOR");
		case 266: return copyString("L_TO");
		case 267: return copyString("L_DO");
		case 268: return copyString("L_BEGIN");
		case 269: return copyString("L_END");
		case 270: return copyString("L_INTEGER");
		case 271: return copyString("L_REAL");
		case 272: return copyString("L_VAR");
		case 273: return copyString("L_ARRAY");
		case 274: return copyString("L_OF");
		case 275: return copyString("L_NOT");
		case 276: return copyString("L_ADD");
		case 277: return copyString("L_SUB");
		case 278: return copyString("L_OR");
		case 279: return copyString("L_MULOP");
		case 280: return copyString("L_RELOP");
		case 281: return copyString("L_ASSIGNOP");
		case 282: return copyString("L_ID");
		case 283: return copyString("L_NUM");
		case 284: return copyString("L_LP");
		case 285: return copyString("L_RP");
		case 286: return copyString("L_LB");
		case 287: return copyString("L_RB");
		case 288: return copyString("L_DD");
		case 289: return copyString("L_D");
		case 290: return copyString("L_COM");
		case 291: return copyString("L_SC");
		case 292: return copyString("L_C");
		default:
		return copyString("");
	}
}
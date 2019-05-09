%{

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lexconstants.h"
#include "parsetree.h"
#include "y.tab.h"

void processLex(int type, char * lexString, bool hasAttribute);


extern int lineNumber;
extern int lineIndex;




%}

comment \(\*(.|\n)*\*\)|\/\/.*
spaces [ \t]+

mulop [*/]|div|mod|and
relop =|\<\>|\<|\<=|\>=|\>

assignop :=

id [a-zA-Z]([a-zA-Z]|[0-9])*
num [0-9]+(E[+-]?[0-9]+)?

%%

"program"			{printf("[PROGRAM]"); processLex(L_PROGRAM, yytext, false);return L_PROGRAM;}
"function"			{printf("[FUNCTION]"); processLex(L_FUNCTION, yytext, false);return L_FUNCTION;}
"procedure"			{printf("[PROCEDURE]"); processLex(L_PROCEDURE, yytext, false);return L_PROCEDURE;}
"if"				{printf("[IF]"); processLex(L_IF, yytext, false);return L_IF;}
"then"				{printf("[THEN]");  processLex(L_THEN, yytext, false);return L_THEN;}
"else"				{printf("[ELSE]");  processLex(L_ELSE, yytext, false);return L_ELSE;}
"while"				{printf("[WHILE]");  processLex(L_WHILE, yytext, false);return L_WHILE;}
"for"				{printf("[FOR]");  processLex(L_FOR, yytext, false);return L_FOR;}
"to"				{printf("[TO]");  processLex(L_TO, yytext, false);return L_TO;}
"do"				{printf("[DO]");  processLex(L_DO, yytext, false);return L_DO;}
"begin"				{printf("[BEGIN]");  processLex(L_BEGIN, yytext, false);return L_BEGIN;}
"end"				{printf("[END]");  processLex(L_END, yytext, false);return L_END;}
"integer"			{printf("[INTEGER]");  processLex(L_INTEGER, yytext, false);return L_INTEGER;}
"real"				{printf("[REAL]");  processLex(L_REAL, yytext, false);return L_REAL;}
"var"				{printf("[VAR]");  processLex(L_VAR, yytext, false);return L_VAR;}
"array"				{printf("[ARRAY]");  processLex(L_ARRAY, yytext, false);return L_ARRAY;}
"of"				{printf("[OF]");  processLex(L_OF, yytext, false);return L_OF;}
"not"				{printf("[NOT]");  processLex(L_NOT, yytext, true);return L_NOT;}
"+"					{printf("[ADD]");  processLex(L_ADD, yytext, true);return L_ADD;}
"-"					{printf("[SUB]");  processLex(L_SUB, yytext, true);return L_SUB;}
"or"				{printf("[OR]");  processLex(L_OR, yytext, true);return L_OR;}
{mulop}				{printf("[MULOP:%s]",yytext); 	processLex(L_MULOP, yytext, true);return L_MULOP;}
{relop}				{printf("[RELOP:%s]",yytext);  	processLex(L_RELOP, yytext, true);return L_RELOP;}
{assignop}			{printf("[ASSIGNOP]");  processLex(L_ASSIGNOP, yytext, false);return L_ASSIGNOP;}
{id}				{printf("[ID:%s]",yytext);  processLex(L_ID, yytext, true);return L_ID;}
{num}				{printf("[NUM:%s]",yytext);  processLex(L_NUM, yytext, true);return L_NUM;}
"("					{printf("[LP]");  processLex(L_LP, yytext, false);return L_LP;}
")"					{printf("[RP]");  processLex(L_RP, yytext, false);return L_RP;}
"["					{printf("[LB]");  processLex(L_LB, yytext, false);return L_LB;}
"]"					{printf("[RB]");  processLex(L_RB, yytext, false);return L_RB;}
".."				{printf("[DD]");  processLex(L_DD, yytext, false);return L_DD;}
"."					{printf("[D]");  processLex(L_D, yytext, false);return L_D;}
","					{printf("[COM]");  processLex(L_COM, yytext, false);return L_COM;}
";"					{printf("[SC]");  processLex(L_SC, yytext, false);return L_SC;}
":"					{printf("[C]");  processLex(L_C, yytext, false);return L_C;}
{spaces} 			{processLexString(yytext);}
{comment}			{processLexString(yytext);}
\n					{printf("[EOL]");  processLexString(yytext);}
. 					{lexError(yytext);}

%%



int yyerror(char * msg) {
   printf("\n[ERROR] PARSER at (%i:%i): %s\n", lineNumber, lineIndex, msg);
   exit(1);
}

int yywrap(void) {
	return 1; 
}


void processLex(int type, char * lexString, bool hasAttribute) {
	TreeValue *  treeValue = parseTreeInitTreeValue(type, hasAttribute? lexString : 0, lineNumber, lineIndex);
	yylval.treeVal = treeValue;
	processLexString(yytext);
}

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

"program"			{printf("[PROGRAM]"); processLex(LL_PROGRAM, yytext, false);return L_PROGRAM;}
"function"			{printf("[FUNCTION]"); processLex(LL_FUNCTION, yytext, false);return L_FUNCTION;}
"procedure"			{printf("[PROCEDURE]"); processLex(LL_PROCEDURE, yytext, false);return L_PROCEDURE;}
"if"				{printf("[IF]"); processLex(LL_IF, yytext, false);return L_IF;}
"then"				{printf("[THEN]");  processLex(LL_THEN, yytext, false);return L_THEN;}
"else"				{printf("[ELSE]");  processLex(LL_ELSE, yytext, false);return L_ELSE;}
"while"				{printf("[WHILE]");  processLex(LL_WHILE, yytext, false);return L_WHILE;}
"for"				{printf("[FOR]");  processLex(LL_FOR, yytext, false);return L_FOR;}
"to"				{printf("[TO]");  processLex(LL_TO, yytext, false);return L_TO;}
"do"				{printf("[DO]");  processLex(LL_DO, yytext, false);return L_DO;}
"begin"				{printf("[BEGIN]");  processLex(LL_BEGIN, yytext, false);return L_BEGIN;}
"end"				{printf("[END]");  processLex(LL_END, yytext, false);return L_END;}
"integer"			{printf("[INTEGER]");  processLex(LL_INTEGER, yytext, false);return L_INTEGER;}
"real"				{printf("[REAL]");  processLex(LL_REAL, yytext, false);return L_REAL;}
"var"				{printf("[VAR]");  processLex(LL_VAR, yytext, false);return L_VAR;}
"array"				{printf("[ARRAY]");  processLex(LL_ARRAY, yytext, false);return L_ARRAY;}
"of"				{printf("[OF]");  processLex(LL_OF, yytext, false);return L_OF;}
"not"				{printf("[NOT]");  processLex(LL_NOT, yytext, true);return L_NOT;}
"+"					{printf("[ADD]");  processLex(LL_ADD, yytext, true);return L_ADD;}
"-"					{printf("[SUB]");  processLex(LL_SUB, yytext, true);return L_SUB;}
"or"				{printf("[OR]");  processLex(LL_OR, yytext, true);return L_OR;}
{mulop}				{printf("[MULOP:%s]",yytext); 	processLex(LL_MULOP, yytext, true);return L_MULOP;}
{relop}				{printf("[RELOP:%s]",yytext);  	processLex(LL_RELOP, yytext, true);return L_RELOP;}
{assignop}			{printf("[ASSIGNOP]");  processLex(LL_ASSIGNOP, yytext, false);return L_ASSIGNOP;}
{id}				{printf("[ID:%s]",yytext);  processLex(LL_ID, yytext, true);return L_ID;}
{num}				{printf("[NUM:%s]",yytext);  processLex(LL_NUM, yytext, true);return L_NUM;}
"("					{printf("[LP]");  processLex(LL_LP, yytext, false);return L_LP;}
")"					{printf("[RP]");  processLex(LL_RP, yytext, false);return L_RP;}
"["					{printf("[LB]");  processLex(LL_LB, yytext, false);return L_LB;}
"]"					{printf("[RB]");  processLex(LL_RB, yytext, false);return L_RB;}
".."				{printf("[DD]");  processLex(LL_DD, yytext, false);return L_DD;}
"."					{printf("[D]");  processLex(LL_D, yytext, false);return L_D;}
","					{printf("[COM]");  processLex(LL_COM, yytext, false);return L_COM;}
";"					{printf("[SC]");  processLex(LL_SC, yytext, false);return L_SC;}
":"					{printf("[C]");  processLex(LL_C, yytext, false);return L_C;}
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

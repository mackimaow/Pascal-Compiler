#include <string.h>
#include "code_generator.h"
#include "utils.h"
#include "linkedlist.h"
#include "lexconstants.h"
#include "object.h"
#include "parsetree.h"
#include "symboltable.h"


static char * startString = 
"\n"
"section .text\n"
"    global _start\n"
"_start:\n"
"\n";

static char * endString = 
"\n"
"	mov eax, 1\n"
"	mov ebx, 0\n"
"	int 0x80\n"
"\n"
"\n";

static char * header = 
"\n"
"	push ebp\n"
"	mov ebp, esp\n"
"\n";

static char * footer = 
"\n"
"	mov esp, ebp\n"
"	pop ebp\n"
"	ret\n"
"\n";

static char * writeCommandString = 
"_write: \n"
"	push ebp \n"
"	mov ebp, esp \n"
" \n"
"	mov eax, [ebp+8]		; grab the first argument \n"
"	push eax 				; stores the entire value to use  (-4) \n"
" \n"
"	cmp eax, -1 \n"
"	jg  _write_store_digits \n"
"	mov edx, 0 \n"
"	mov ecx, -1				;  \n"
"	mul ecx					; negate value \n"
"	mov [ebp-4], eax		; place back into register			 \n"
" \n"
"	push '-' \n"
" \n"
"	mov ecx, esp \n"
"	mov eax, 4              ; SYS_WRITE \n"
"    mov edx, 1              ; Amount of chars to print \n"
"    mov ebx, 1              ; STDOUT \n"
"    int 0x80 \n"
" \n"
"    pop eax \n"
" \n"
"_write_store_digits: \n"
" \n"
"	mov edx, 0 \n"
"	mov eax, [ebp-4]		; set up number to divide \n"
"	mov ebx, 10				; set up divider \n"
"	div ebx					; eax = eax / ebx    edx = eax mod ebx  \n"
"	add edx, '0' \n"
"	mov [ebp-4], eax		; \n"
"	push edx				; \n"
" \n"
"    cmp eax, 0  \n"
"jne _write_store_digits \n"
" \n"
"_write_digits: \n"
"    mov ecx, esp \n"
"	mov eax, 4              ; SYS_WRITE \n"
"    mov edx, 1              ; Amount of chars to print \n"
"    mov ebx, 1              ; STDOUT \n"
"    int 0x80 \n"
" \n"
"    pop eax \n"
"    mov eax, ebp \n"
"    cmp eax, esp \n"
"jne _write_digits \n"
" \n"
"    mov eax, 0xa \n"
"    push eax \n"
"    mov ecx, esp \n"
"	mov eax, 4              ; SYS_WRITE \n"
"    mov edx, 1              ; Amount of chars to print \n"
"    mov ebx, 1              ; STDOUT \n"
"    int 0x80 \n"
"    pop eax \n"
" \n"
"    mov eax, 0xd \n"
"    push eax \n"
"    mov ecx, esp \n"
"	mov eax, 4              ; SYS_WRITE \n"
"    mov edx, 1              ; Amount of chars to print \n"
"    mov ebx, 1              ; STDOUT \n"
"    int 0x80 \n"
"    pop eax \n"
" \n"
"	mov esp, ebp \n"
"	pop ebp \n"
"	ret \n";


static char * comparingOperators =
"\n\n"
"_less_than:\n"
"	mov eax, -1\n"
"	cmp ebx, ecx  \n"
"	jl _less_than_jump\n"
"	mov eax, 0\n"
"_less_than_jump:\n"
"	ret\n"
"\n"
"_greater_than:\n"
"	mov eax, -1\n"
"	cmp ebx, ecx  \n"
"	jg _greater_than_jump\n"
"	mov eax, 0\n"
"_greater_than_jump:\n"
"	ret\n"
"\n"
"\n"
"_greater_than_equal:\n"
"	mov eax, -1\n"
"	cmp ebx, ecx  \n"
"	jge _greater_than_equal_jump\n"
"	mov eax, 0\n"
"_greater_than_equal_jump:\n"
"	ret\n"
"\n"
"_less_than_equal:\n"
"	mov eax, -1\n"
"	cmp ebx, ecx  \n"
"	jle _less_than_equal_jump\n"
"	mov eax, 0\n"
"_less_than_equal_jump:\n"
"	ret\n"
"\n"
"_not_equal:\n"
"	mov eax, -1\n"
"	cmp ebx, ecx  \n"
"	jne _not_equal_jump\n"
"	mov eax, 0\n"
"_not_equal_jump:\n"
"	ret\n"
"\n"
"_equal:\n"
"	mov eax, -1\n"
"	cmp ebx, ecx  \n"
"	je _equal_jump\n"
"	mov eax, 0\n"
"_equal_jump:\n"
"	ret\n"
"\n\n";


static char * array_out_of_bounds = 
"\n"
"\nsection	.data\n"
"\n	msg db '[ERROR] ARRAY OUT OF BOUNDS!', 0xa  \n"
"\n	len equ $ - msg     \n";

static char * errorFunction =
"\n"
"_error:	            ;tells linker entry point\n"
"	push eax 		\n"
"	push edx 		\n"
"	 				\n"
"	mov	edx,len     ;message length\n"
"	mov	ecx,msg     ;message to write\n"
"	mov	ebx,1       ;file descriptor (stdout)\n"
"	mov	eax,4       ;system call number (sys_write)\n"
"	int	0x80        ;call kernel\n"
"	 				\n"
"	call _write 	\n"
"	pop edx 		\n"
"	call _write 	\n"
"	 				\n"
"	mov	eax,1       ;system call number (sys_exit)\n"
"	mov	ebx,1       ; error 				\n"
"	int	0x80        ;call kernel\n";

// static char * writePreMadeMessageString =
// "\n"
// "_write_msg:	    ;tells linker entry point\n"
// "   mov	ebx,1       ;file descriptor (stdout)\n"
// "   mov	eax,4       ;system call number (sys_write)\n"
// "   int	0x80        ;call kernel\n"




static ListPrintProperties codePrint = {"", "", ""};
static ListPrintProperties callPrint = {"__", "_", ""};
static ListPrintProperties labelPrint = {"__", "_", "_"};

static void appendString(CodeGenerator * codeGenerator, char * string);
static void prependString(CodeGenerator * codeGenerator, char * string);
static void safeSwap(LinkedList * tempStack);
static int safePop(LinkedList * tempStack);
static void safePush(LinkedList * tempStack, int value);
static int safePeak(LinkedList * tempStack);
static void appendRegBPString(CodeGenerator * codeGenerator, int indexFromBP);
// static void writePreMadeMessage();

static int labelVariables(Tree * variableList, bool areParameters);

static void genCodeProgram(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * program);
static void genCodeSubPrograms(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * subPrograms);
static void genCodeProcedure(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * procedure);
static void genCodeFunction(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * function);
static void genCodeStatements(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * statements, LinkedList * tempStack);
static void genCodeStatement(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * statement, LinkedList * tempStack);
static void genCodeTwoNodeOperation(CodeGenerator * codeGenerator, char * operator);

static LinkedList * generateFullHeader(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * declarations, int lastRegisterUnsed);
static void generateLocalVariables(CodeGenerator * codeGenerator, Tree * declarations);

static void getCallingVariablePutInEAX(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * variable);
static void callFunctionPutInEAX(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * functionCall, LinkedList * tempStack );
static void callProcedure(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * procedureCall, LinkedList * tempStack);
static void callWriteProcedure(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * procedureCall );
static void assignFromEAXToVariable(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * variable, LinkedList * tempStack, bool isReturnType);
static void genCodeIndexVariableAtEAX(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * variable);
static bool genCodeExpressionToEAX(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * expression, LinkedList * tempStack);
static int genCodePushArrayVariable(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * arrayType, Tree * variable);

char * generateCode(Tree * tree, SymbolTable * symbolTable) {
	CodeGenerator * codeGenerator = linkedListInitWithPrintProperties(&STRING_OBJECT, &codePrint);
	appendString(codeGenerator, startString);
	char * programName = parseTreeGetAttribute(tree);
	char * callLabel = symbolTableScopeTraceString(symbolTable, &callPrint);
	appendString(codeGenerator, "	call ");
	appendString(codeGenerator, callLabel);
	free(callLabel);
	appendString(codeGenerator, programName);
	appendString(codeGenerator, "\n");
	appendString(codeGenerator, endString);
	appendString(codeGenerator, writeCommandString);
	appendString(codeGenerator, comparingOperators);
	genCodeProgram(symbolTable, codeGenerator, tree);
	appendString(codeGenerator, errorFunction);
	prependString(codeGenerator, array_out_of_bounds);

	char * code = linkedListToString(codeGenerator);

	linkedListDestroy(codeGenerator);
	return code;
}


// static void writePreMadeMessage(CodeGenerator * codeGenerator, char * string) {


// }



static LinkedList * generateFullHeader(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * declarations, int lastRegisterUnsed) {
	appendString(codeGenerator,"\n");
	char * callLabel = symbolTableScopeTraceString(symbolTable, &callPrint);
	appendString(codeGenerator,callLabel);
	appendString(codeGenerator, ":\n\n");
	appendString(codeGenerator, header);
	free(callLabel);

	appendString(codeGenerator, "	push eax	; base pointer of parent \n");

	generateLocalVariables(codeGenerator, declarations);
	// for (int i = -8; i > lastRegisterUnsed; i-=4 ) {
	// 	appendString(codeGenerator, "	push 0		; variable register \n");
	// }
	int numberOfTempRegs = symbolTablegetNumTempRegs(symbolTable);
	int lastLastRegisterUsed = lastRegisterUnsed - 4 * numberOfTempRegs;

	LinkedList * tempStack = linkedListInit(&INT_OBJECT);
	for (int i = lastRegisterUnsed; i > lastLastRegisterUsed; i-= 4) {
		safePush(tempStack, i);
		appendString(codeGenerator, "	push 0		; temporary register \n");	
	}
	return tempStack;
}

static void generateLocalVariables(CodeGenerator * codeGenerator, Tree * declarations) {
	Iterator * typeIterator = iteratorInit(treeGetChildren(declarations));
	appendString(codeGenerator, "\n");
	while(iteratorHasNext(typeIterator)) {
		Tree * identifier_list = iteratorGetNext(typeIterator);
		Tree * variable_type = iteratorGetNext(typeIterator);
		int variableTypeValue = parseTreeGetType(variable_type);


		if (variableTypeValue == LL_ARRAY) {
			Tree * lowerBoundLocation = treeGetChild(variable_type, 0);
			Tree * upperBoundLocation = treeGetChild(variable_type, 1);

			int lowerBound = parseTreeGetLabel(lowerBoundLocation);
			int upperBound = parseTreeGetLabel(upperBoundLocation);
			char * lowerBoundString = intToString(lowerBound);
			char * upperBoundString = intToString(upperBound);

			Iterator * iterator = iteratorInit(treeGetChildren(identifier_list));
			while (iteratorHasNext(iterator)) {
				Tree * variable = iteratorGetNext(iterator);
				appendString(codeGenerator, "	push ");
				appendString(codeGenerator, lowerBoundString);
				appendString(codeGenerator, "		; lower bound array\n");
				appendString(codeGenerator, "	push ");
				appendString(codeGenerator, upperBoundString);
				appendString(codeGenerator, "		; upper bound array\n");
				for(int i = lowerBound; i <= upperBound; i++)
					appendString(codeGenerator, "	push 0		; declared array element \n");
			}
			free(lowerBoundString);
			free(upperBoundString);
			iteratorDestroy(iterator);
		} else {
			Iterator * iterator = iteratorInit(treeGetChildren(identifier_list));
			while (iteratorHasNext(iterator)) {
				iteratorGetNext(iterator);
				appendString(codeGenerator, "	push 0		; declared variable\n");
			}
			iteratorDestroy(iterator);
		}
	}
	appendString(codeGenerator, "\n");

	iteratorDestroy(typeIterator);
}

static void genCodeProgram(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * program) {
	char * program_name = parseTreeGetAttribute(program);
	symbolTablePushScope(symbolTable, program_name);

	Tree * parameters = treeGetChild(program, 0); // skip 
	Tree * declarations = treeGetChild(program, 1);
	Tree * subprogram_declarations = treeGetChild(program, 2);
	Tree * statement_list = treeGetChild(program, 3);

	int lastRegisterUsed = labelVariables(declarations, false);
	genCodeSubPrograms(symbolTable, codeGenerator, subprogram_declarations);
	// gen call here

	LinkedList * tempStack = generateFullHeader(symbolTable, codeGenerator, declarations, lastRegisterUsed); 
	genCodeStatements(symbolTable, codeGenerator, statement_list, tempStack);
	appendString(codeGenerator, footer);

	linkedListDestroy(tempStack);
	symbolTablePopScope(symbolTable);
}

static void genCodeSubPrograms(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * subPrograms) {
	int listSize = parseTreeGetLabel(subPrograms);
	if(listSize == 0)
		return;
	Iterator * iterator = iteratorInit(treeGetChildren(subPrograms));
	while(iteratorHasNext(iterator)){
		Tree * subProgram = (Tree *) iteratorGetNext(iterator);
			int type = parseTreeGetType(subProgram);
			bool isFunction = type == LL_FUNCTION;
			if(isFunction)
				genCodeFunction(symbolTable, codeGenerator, subProgram);
			else
				genCodeProcedure(symbolTable, codeGenerator, subProgram);

	}
	iteratorDestroy(iterator);
}

static void genCodeProcedure(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * procedure) {
	char * procedure_name = parseTreeGetAttribute(procedure);
	symbolTablePushScope(symbolTable, procedure_name);

	Tree * parameters = treeGetChild(procedure, 0);
	Tree * declarations = treeGetChild(procedure, 1);
	Tree * subprogram_declarations = treeGetChild(procedure, 2);
	Tree * statement_list = treeGetChild(procedure, 3);

	labelVariables(parameters, true);
	int lastRegisterUsed = labelVariables(declarations, false);
	genCodeSubPrograms(symbolTable, codeGenerator, subprogram_declarations);

	// gen call here
	LinkedList * tempStack = generateFullHeader(symbolTable, codeGenerator, declarations, lastRegisterUsed); 
	genCodeStatements(symbolTable, codeGenerator, statement_list, tempStack);
	appendString(codeGenerator, footer);

	linkedListDestroy(tempStack);
	symbolTablePopScope(symbolTable);
}

static void genCodeFunction(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * function) {

	char * function_name = parseTreeGetAttribute(function);
	symbolTablePushScope(symbolTable, function_name);
	Tree * parameters = treeGetChild(function, 0); // skip 
	Tree * declarations = treeGetChild(function, 2);
	Tree * subprogram_declarations = treeGetChild(function, 3);
	Tree * statement_list = treeGetChild(function, 4);

	int returnLocation = labelVariables(parameters, true);
	parseTreeSetLabel(function, returnLocation);
	int lastRegisterUsed = labelVariables(declarations, false);

	genCodeSubPrograms(symbolTable, codeGenerator, subprogram_declarations);

	// gen call here
	LinkedList * tempStack = generateFullHeader(symbolTable, codeGenerator, declarations, lastRegisterUsed); 
	genCodeStatements(symbolTable, codeGenerator, statement_list, tempStack);
	char * function_end = symbolTableScopeTraceString(symbolTable, &labelPrint);
	appendString(codeGenerator, "\n");
	appendString(codeGenerator, function_end);
	appendString(codeGenerator, "end:\n");
	appendString(codeGenerator, footer);
	free(function_end);
	linkedListDestroy(tempStack);
	symbolTablePopScope(symbolTable);
}

static void genCodeStatements(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * statements, LinkedList * tempStack) {
	int numberOfStatements = parseTreeGetLabel(statements);

	if(numberOfStatements == 0)
		return;
	Iterator * iterator = iteratorInit(treeGetChildren(statements));
	while (iteratorHasNext(iterator)) {
		Tree * statement = (Tree *) iteratorGetNext(iterator);
		genCodeStatement(symbolTable, codeGenerator, statement, tempStack);
	}
	iteratorDestroy(iterator);
}

static void genCodeStatement(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * statement, LinkedList * tempStack) {
	int type = parseTreeGetType(statement);
	int numberOfRegisters;

	switch (type) {
		case LL_ASSIGNOP:		
			;
			Tree * variable = treeGetChild(statement, 0);
			Tree * expression = treeGetChild(statement, 1);
			genCodeExpressionToEAX(symbolTable, codeGenerator, expression, tempStack);
			int label = parseTreeGetLabel(statement);
			assignFromEAXToVariable(symbolTable, codeGenerator, variable, tempStack, label == 1);
			if (label == 1) { // return function
				char * function_end = symbolTableScopeTraceString(symbolTable, &labelPrint);
				appendString(codeGenerator, "\n	jmp ");
				appendString(codeGenerator, function_end);
				appendString(codeGenerator, "end\n");
				free(function_end);
			}
			break;
		case READ_PROCEDURE_ID:
			break;
		case WRITE_PROCEDURE_ID:
			;
			Tree * write_argumentList = treeGetChild(statement, 0);
			Iterator * write_arguments_iterator = iteratorInit(treeGetChildren(write_argumentList));
			appendString(codeGenerator, "\n	push 0\n");
			while (iteratorHasNext(write_arguments_iterator)) {
				Tree * arg = iteratorGetNext(write_arguments_iterator);
				genCodeExpressionToEAX(symbolTable, codeGenerator, arg, tempStack);
				appendString(codeGenerator, "\n	mov [esp],eax\n");
				appendString(codeGenerator, "	call _write");
			}
			appendString(codeGenerator, "\n	pop eax\n");
			iteratorDestroy(write_arguments_iterator);
			break;
		case LL_ID: // procedure call
			callProcedure(symbolTable, codeGenerator, statement, tempStack);
			break;
		case LL_IF:
			;
			Tree * if_condition = treeGetChild(statement, 0);
			char *  if_label_string = intToString( parseTreeGetLabel(statement));
			genCodeExpressionToEAX(symbolTable, codeGenerator, if_condition, tempStack);
			char * temp =
				"\n	cmp eax, 0\n"
				"\n	je ";
			appendString(codeGenerator, temp);
			char * if_label = symbolTableScopeTraceString(symbolTable, &labelPrint);
			appendString(codeGenerator, if_label);
			appendString(codeGenerator, "else_");
			appendString(codeGenerator, if_label_string);
			appendString(codeGenerator, "\n");

			genCodeStatement(symbolTable, codeGenerator,  treeGetChild(statement, 1), tempStack);
			if (treeGetSize(statement) == 2) { // if then 
				appendString(codeGenerator, "\n");
				appendString(codeGenerator, if_label);
				appendString(codeGenerator, "else_");
				appendString(codeGenerator, if_label_string);
				appendString(codeGenerator, ":\n");
			} else { // if then else
				appendString(codeGenerator, "\n	jmp ");
				appendString(codeGenerator, if_label);
				appendString(codeGenerator, "end_if_");
				appendString(codeGenerator, if_label_string);

				appendString(codeGenerator, "\n");
				appendString(codeGenerator, if_label);
				appendString(codeGenerator, "else_");
				appendString(codeGenerator, if_label_string);
				appendString(codeGenerator, ":\n");
				genCodeStatement(symbolTable, codeGenerator,  treeGetChild(statement, 2), tempStack);
				appendString(codeGenerator, "\n");
				appendString(codeGenerator, if_label);
				appendString(codeGenerator, "end_if_");
				appendString(codeGenerator, if_label_string);
				appendString(codeGenerator, ":\n");
			}
			free(if_label);
			free(if_label_string);
			break;
		case LL_WHILE:
			;
			Tree * while_condition = treeGetChild(statement, 0);
			char *  while_label_string = intToString( parseTreeGetLabel(statement));
			char * while_label = symbolTableScopeTraceString(symbolTable, &labelPrint);
			appendString(codeGenerator, "\n");
			appendString(codeGenerator, while_label);
			appendString(codeGenerator, "while_");
			appendString(codeGenerator, while_label_string);
			appendString(codeGenerator, ":\n");
			genCodeExpressionToEAX(symbolTable, codeGenerator, while_condition, tempStack);
			temp =
				"\n	cmp eax, 0\n"
				"\n	je ";
			appendString(codeGenerator, temp);
			appendString(codeGenerator, while_label);
			appendString(codeGenerator, "while_end_");
			appendString(codeGenerator, while_label_string);
			appendString(codeGenerator, "\n");
			genCodeStatement(symbolTable, codeGenerator, treeGetChild(statement, 1), tempStack);
			appendString(codeGenerator, "\n	jmp ");
			appendString(codeGenerator, while_label);
			appendString(codeGenerator, "while_");
			appendString(codeGenerator, while_label_string);
			appendString(codeGenerator, "\n");
			appendString(codeGenerator, while_label);
			appendString(codeGenerator, "while_end_");
			appendString(codeGenerator, while_label_string);
			appendString(codeGenerator, ":\n");
			free(while_label);
			free(while_label_string);
			break;
		case LL_FOR:
			;
			Tree * iteratorVariable = treeGetChild(statement, 0);
			Tree * firstExpression  = treeGetChild(statement, 1);
			Tree * secondExpression = treeGetChild(statement, 2);
			Tree * forBody = treeGetChild(statement, 3);

			char * for_label_string = intToString( parseTreeGetLabel(statement));
			char * for_label = symbolTableScopeTraceString(symbolTable, &labelPrint);
			genCodeExpressionToEAX(symbolTable, codeGenerator, firstExpression, tempStack);
			appendString(codeGenerator, "\n	push eax\n");
			assignFromEAXToVariable(symbolTable, codeGenerator, iteratorVariable, tempStack, false);

			genCodeExpressionToEAX(symbolTable, codeGenerator, secondExpression, tempStack);
			appendString(codeGenerator, "\n	push eax\n");
			appendString(codeGenerator, "\n	mov eax,1\n	mov ebx,[esp+4]\n	mov ecx,[esp]\n	cmp ebx,ecx\n	jle ");
			appendString(codeGenerator, for_label);
			appendString(codeGenerator, "for_incr_");
			appendString(codeGenerator, for_label_string);
			appendString(codeGenerator, "\n");
			appendString(codeGenerator, "\n	mov eax,-1\n");
			appendString(codeGenerator, "\n");
			appendString(codeGenerator, for_label);
			appendString(codeGenerator, "for_incr_");
			appendString(codeGenerator, for_label_string);
			appendString(codeGenerator, ":\n");
			appendString(codeGenerator, "\n	mov [esp+4],eax\n");
			appendString(codeGenerator, for_label);
			appendString(codeGenerator, "for_");
			appendString(codeGenerator, for_label_string);
			appendString(codeGenerator, ":\n");
			genCodeStatement(symbolTable, codeGenerator, forBody, tempStack);
			
			getCallingVariablePutInEAX(symbolTable, codeGenerator, iteratorVariable);
			appendString(codeGenerator, "\n	mov ebx,[esp]\n 	cmp eax,ebx\n	je ");
			appendString(codeGenerator, for_label);
			appendString(codeGenerator, "for_end_");
			appendString(codeGenerator, for_label_string);
			appendString(codeGenerator, "\n");
			
			appendString(codeGenerator, "\n	mov ebx,[esp+4]\n 	add eax,ebx\n");
			assignFromEAXToVariable(symbolTable, codeGenerator, iteratorVariable, tempStack, false);

			appendString(codeGenerator, "\n	jmp ");
			appendString(codeGenerator, for_label);
			appendString(codeGenerator, "for_");
			appendString(codeGenerator, for_label_string);
			appendString(codeGenerator, "\n");

			appendString(codeGenerator, for_label);
			appendString(codeGenerator, "for_end_");
			appendString(codeGenerator, for_label_string);
			appendString(codeGenerator, ":\n");
			appendString(codeGenerator, "\n	pop eax\n");
			appendString(codeGenerator, "\n	pop eax\n");
			free(for_label);
			free(for_label_string);
			break;
		default:
			;
			genCodeStatements(symbolTable, codeGenerator, statement, tempStack);
		break;
	}
}

static void getCallingVariablePutInEAX(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * variable) {
	char * variableName = parseTreeGetAttribute(variable);
	SearchResult searchResult;
	symbolTableSearchAll(symbolTable, variableName, &searchResult);
	int i = searchResult.searchDepth;

	appendString(codeGenerator, "\n	mov ebx,ebp\n");
	for(int j = 0; j < i; j++)
		appendString(codeGenerator, "\n	mov ecx,[ebx-4]\n	mov ebx,ecx\n");
	Tree * variableType = searchResult.attributes->variableValue;
	int variableOffset = parseTreeGetLabel(variableType);
	appendString(codeGenerator, "\n	add ebx,");
	char * variableOffsetString = intToString(variableOffset);
	appendString(codeGenerator, variableOffsetString);
	free(variableOffsetString);
	appendString(codeGenerator, "\n	mov eax,[ebx]\n");
}

static void assignFromEAXToVariable(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * variable, LinkedList * tempStack, bool isReturnType) {
	appendString(codeGenerator, "\n	push eax\n");

	if ( ! treeIsLeaf(variable) ) {
		Tree * index = treeGetChild(variable, 0);
		genCodeExpressionToEAX(symbolTable, codeGenerator, index, tempStack);
		appendString(codeGenerator, "	mov edx,eax\n");
	}
	char * variableName = parseTreeGetAttribute(variable);
	SearchResult searchResult;
	symbolTableSearchAll(symbolTable, variableName, &searchResult);
	int i = searchResult.searchDepth;

	appendString(codeGenerator, "	mov ebx,ebp\n");

	Tree * variableType;
	if (!isReturnType) { 
		for(int j = 0; j < i; j++)
			appendString(codeGenerator, "	mov ecx,[ebx-4]\n	mov ebx,ecx\n");
		variableType = searchResult.attributes->variableValue;
	} else {
		variableType = symbolTablePeakScope(symbolTable)->scopeLocation;
	}
	int variableOffset = parseTreeGetLabel(variableType);
	appendString(codeGenerator, "	add ebx,");
	char * variableOffsetString = intToString(variableOffset);
	appendString(codeGenerator, variableOffsetString);
	free(variableOffsetString);
	appendString(codeGenerator, "\n");

	if ( ! treeIsLeaf(variable) ) {
		appendString(codeGenerator, "	mov eax,[ebx]\n	cmp edx,eax\n	jl _error\n");
		appendString(codeGenerator, "	mov eax,[ebx-4]\n	cmp edx,eax\n	jg _error\n");
		appendString(codeGenerator, "	mov eax,[ebx]\n	sub edx,eax\n");
		appendString(codeGenerator, "	sub ebx,8\n	mov eax,edx \n	mov edx,4 \n	mul edx\n	 sub ebx,eax\n");
	}
	appendString(codeGenerator, "	pop eax\n	mov [ebx],eax \n");
}

static void genCodeIndexVariableAtEAX(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * variable) { // indexing arrays

	appendString(codeGenerator, "\n	mov edx,eax\n");
	char * variableName = parseTreeGetAttribute(variable);
	SearchResult searchResult;
	symbolTableSearchAll(symbolTable, variableName, &searchResult);
	int i = searchResult.searchDepth;

	appendString(codeGenerator, "	mov ebx,ebp\n");
	for(int j = 0; j < i; j++)
		appendString(codeGenerator, "	mov ecx,[ebx-4]\n	mov ebx,ecx\n");
	Tree * variableType = searchResult.attributes->variableValue;
	int variableOffset = parseTreeGetLabel(variableType);
	appendString(codeGenerator, "	add ebx,");
	char * variableOffsetString = intToString(variableOffset);
	appendString(codeGenerator, variableOffsetString);
	free(variableOffsetString);
	appendString(codeGenerator, "\n");

	appendString(codeGenerator, "	mov eax,[ebx]\n	cmp edx,eax\n	jl _error\n");
	appendString(codeGenerator, "	mov eax,[ebx-4]\n	cmp edx,eax\n	jg _error\n");
	appendString(codeGenerator, "	mov eax,[ebx]\n	sub edx,eax\n");
	appendString(codeGenerator, "	sub ebx,8\n	mov eax,edx \n	mov edx,4 \n	mul edx\n	 sub ebx,eax\n");

	appendString(codeGenerator, "	mov eax,[ebx] \n");
}

static int genCodePushArrayVariable(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * arrayType, Tree * variable) { // indexing arrays
	int lowerBound = parseTreeGetLabel(treeGetChild(arrayType, 0));
	int upperBound = parseTreeGetLabel(treeGetChild(arrayType, 1));
	int arraySize = upperBound - lowerBound + 1;

	char * variableName = parseTreeGetAttribute(variable);
	SearchResult searchResult;
	symbolTableSearchAll(symbolTable, variableName, &searchResult);
	int i = searchResult.searchDepth;

	appendString(codeGenerator, "	mov ebx,ebp\n");
	for(int j = 0; j < i; j++)
		appendString(codeGenerator, "	mov ecx,[ebx-4]\n	mov ebx,ecx\n");
	Tree * variableType = searchResult.attributes->variableValue;
	int variableOffset = parseTreeGetLabel(variableType);
	appendString(codeGenerator, "	add ebx,");
	char * variableOffsetString = intToString(variableOffset);
	appendString(codeGenerator, variableOffsetString);
	free(variableOffsetString);
	appendString(codeGenerator, "\n");


	appendString(codeGenerator, "	mov eax,[ebx]\n	push eax\n	mov eax,[ebx-4]\n	push eax\n");

	for(int i = 0; i < arraySize; i++) {
		char * indexValue = intToString(i * 4 + 8);
		appendString(codeGenerator, "	mov eax,[ebx-");
		appendString(codeGenerator, indexValue);
		appendString(codeGenerator, "]\n");
		appendString(codeGenerator, "	push eax\n");
		free(indexValue);
	}
	return arraySize + 2;
}


// TODO : call function, procedure, write tommorrow
static void callFunctionPutInEAX(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * functionCall, LinkedList * tempStack ) {
	char * functionName = parseTreeGetAttribute(functionCall);

	SearchResult searchResult;
	bool foundResult = symbolTableSearchAll(symbolTable, functionName, &searchResult);

	Tree * procedureDefinition;
	int searchDepth;
	if (!foundResult) {
		SymbolTableScope * currentScope = symbolTablePeakScope(symbolTable);
		procedureDefinition = currentScope->scopeLocation;
		searchDepth = 1;
	} else {
		procedureDefinition = searchResult.attributes->variableValue;
		searchDepth = searchResult.searchDepth;
	}
	Tree * parameterList = treeGetChild(procedureDefinition, 0);
	Iterator * parameterListIterator = iteratorInit(treeGetChildren(parameterList));

	Tree * write_argumentList = treeGetChild(functionCall, 0);
	Iterator * write_arguments_iterator = iteratorInit(treeGetChildren(write_argumentList));


	appendString(codeGenerator, "\n	push 0\n"); // push return value
	int numberOfPushedElements = 1;
	while (iteratorHasNext(parameterListIterator)) {
		Tree * identifier_list = iteratorGetNext(parameterListIterator);
		Tree * identifier_type = iteratorGetNext(parameterListIterator);
		int numberOfTypes = treeGetSize(identifier_list);

		int parameterType = parseTreeGetType(identifier_type);

		if (parameterType == LL_ARRAY) { 
			for (int i = 0; i < numberOfTypes; i++) {
				Tree * arg = iteratorGetNext(write_arguments_iterator);
				numberOfPushedElements += genCodePushArrayVariable(symbolTable, codeGenerator, identifier_type, arg);
			}
		} else {
			for (int i = 0; i < numberOfTypes; i++) {
				Tree * arg = iteratorGetNext(write_arguments_iterator);
				genCodeExpressionToEAX(symbolTable, codeGenerator, arg, tempStack);
				appendString(codeGenerator, "\n	push eax\n");
				numberOfPushedElements++;
			}
		}
	}
	iteratorDestroy(write_arguments_iterator);
	iteratorDestroy(parameterListIterator);

	appendString(codeGenerator, "\n	mov eax, ebp\n");
	for (int i = 0; i < searchDepth; i++)
		appendString(codeGenerator, "	mov ecx,[eax-4]\n	mov eax,ecx\n");
	char * callLabel = symbolTableScopeTraceStringAtDepth(symbolTable, searchDepth, &labelPrint);
	appendString(codeGenerator, "\n	call ");
	appendString(codeGenerator, callLabel);
	appendString(codeGenerator, functionName);
	appendString(codeGenerator, "\n");
	for (int i = 0; i < numberOfPushedElements; i++)
		appendString(codeGenerator, "	pop eax\n");

	free(callLabel);	
}

static void callProcedure(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * procedureCall, LinkedList * tempStack) {
	char * procedureName = parseTreeGetAttribute(procedureCall);

	SearchResult searchResult;
	symbolTableSearchAll(symbolTable, procedureName, &searchResult);

	Tree * procedureDefinition = searchResult.attributes->variableValue;
	Tree * parameterList = treeGetChild(procedureDefinition, 0);
	Iterator * parameterListIterator = iteratorInit(treeGetChildren(parameterList));

	Tree * write_argumentList = treeGetChild(procedureCall, 0);
	Iterator * write_arguments_iterator = iteratorInit(treeGetChildren(write_argumentList));

	int numberOfPushedElements = 0;
	while (iteratorHasNext(parameterListIterator)) {
		Tree * identifier_list = iteratorGetNext(parameterListIterator);
		Tree * identifier_type = iteratorGetNext(parameterListIterator);
		int numberOfTypes = treeGetSize(identifier_list);

		int parameterType = parseTreeGetType(identifier_type);

		if (parameterType == LL_ARRAY) { 
			for (int i = 0; i < numberOfTypes; i++) {
				Tree * arg = iteratorGetNext(write_arguments_iterator);
				numberOfPushedElements += genCodePushArrayVariable(symbolTable, codeGenerator, identifier_type, arg);
			}
		} else {
			for (int i = 0; i < numberOfTypes; i++) {
				Tree * arg = iteratorGetNext(write_arguments_iterator);
				genCodeExpressionToEAX(symbolTable, codeGenerator, arg, tempStack);
				appendString(codeGenerator, "\n	push eax\n");
				numberOfPushedElements++;
			}
		}
	}
	iteratorDestroy(write_arguments_iterator);
	iteratorDestroy(parameterListIterator);

	int searchDepth = searchResult.searchDepth;
	appendString(codeGenerator, "\n	mov eax, ebp\n");
	for (int i = 0; i < searchDepth; i++)
		appendString(codeGenerator, "	mov ecx,[eax-4]\n	mov eax,ecx\n");
	char * callLabel = symbolTableScopeTraceStringAtDepth(symbolTable, searchDepth, &labelPrint);
	appendString(codeGenerator, "\n	call ");
	appendString(codeGenerator, callLabel);
	appendString(codeGenerator, procedureName);
	appendString(codeGenerator, "\n");
	for (int i = 0; i < numberOfPushedElements; i++)
		appendString(codeGenerator, "	pop eax\n");

	free(callLabel);
}


static bool genCodeExpressionToEAX(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * expression, LinkedList * tempStack) {
	int expressionType = parseTreeGetType(expression);
	if (expressionType == LL_ID) { // leaf

		char * expressionAttribute = parseTreeGetAttribute(expression);
		SearchResult searchResult;
		bool foundResult = symbolTableSearchAll(symbolTable, expressionAttribute, &searchResult);
		
		Tree * actualTypeLocation;
		if (!foundResult) { // function recusion 
			SymbolTableScope * currentScope = symbolTablePeakScope(symbolTable);
			actualTypeLocation = currentScope->scopeLocation;
		} else {
			actualTypeLocation = searchResult.attributes->variableValue;
		}
		

		int actualType = parseTreeGetType(actualTypeLocation);

		if(actualType != LL_ID) {
			callFunctionPutInEAX(symbolTable, codeGenerator, expression, tempStack );   // calculate function
		} else {
			if ( ! treeIsLeaf(expression) ) {
				Tree * index = treeGetChild(expression, 0);
				genCodeExpressionToEAX(symbolTable, codeGenerator, index, tempStack);
				genCodeIndexVariableAtEAX(symbolTable, codeGenerator, expression);

			} else {
				Tree * typeTypeLocation = searchResult.attributes->variableType;
				int type = parseTreeGetType(typeTypeLocation);
				if (type == LL_ARRAY) {
					return true;
				} else {
					getCallingVariablePutInEAX(symbolTable, codeGenerator, expression);
				}
			}
		}
	} else if (expressionType == LL_NUM) { // leaf
		char * integerValue = parseTreeGetAttribute(expression);
		int i = stringToInt(integerValue);
		char * i2 = intToString(i); // gets rid of exponential form
		appendString(codeGenerator, "\n	mov eax,");
		appendString(codeGenerator, i2);
		appendString(codeGenerator, "\n");
		free(i2);
	} else {
		char * operator = parseTreeGetAttribute(expression);
		int childrenSize = treeGetSize(expression);
		if(childrenSize == 1) { // leaf node hybrid
			Tree * onlyChild = treeGetChild(expression, 0);
			genCodeExpressionToEAX(symbolTable, codeGenerator, onlyChild, tempStack);
			if (strcmp("not", operator) == 0) {
				appendString(codeGenerator, "\n	not eax\n");
			} else { // negate operator
				appendString(codeGenerator, "\n	mov ebx,-1\n");
				appendString(codeGenerator, "	mul ebx\n");
			}
		} else {
			Tree * leftChild = treeGetChild(expression, 0);
			Tree * rightChild = treeGetChild(expression, 1);
			int leftChildLabel = parseTreeGetLabel(leftChild);
			int rightChildLabel = parseTreeGetLabel(rightChild);
			int childType = parseTreeGetType(leftChild);
			if (rightChildLabel == 0) {
				int topStack = safePeak(tempStack);
				genCodeExpressionToEAX(symbolTable, codeGenerator, leftChild, tempStack);
				if (treeGetSize(leftChild) < 2) {
					appendString(codeGenerator, "\n	mov [");
					appendRegBPString(codeGenerator, topStack);
					appendString(codeGenerator, "],eax");
				}
				genCodeExpressionToEAX(symbolTable, codeGenerator, rightChild, tempStack);
				
				appendString(codeGenerator, "\n	mov ecx,eax\n");
				appendString(codeGenerator, "	mov ebx,[");
				appendRegBPString(codeGenerator, topStack);
				appendString(codeGenerator, "]\n");

				genCodeTwoNodeOperation(codeGenerator, operator);
				
				appendString(codeGenerator, "	mov [");
				appendRegBPString(codeGenerator, topStack);
				appendString(codeGenerator, "],eax\n");

			} else if (rightChildLabel > leftChildLabel) {
				safeSwap(tempStack);
				genCodeExpressionToEAX(symbolTable, codeGenerator, rightChild, tempStack);
				int R = safePop(tempStack);
				genCodeExpressionToEAX(symbolTable, codeGenerator, leftChild, tempStack);
				int topStack = safePeak(tempStack);
				if (treeGetSize(leftChild) < 2) {
					appendString(codeGenerator, "\n mov [");
					appendRegBPString(codeGenerator, topStack);
					appendString(codeGenerator, "],eax");
				}
				appendString(codeGenerator, "\n	mov ebx,[");
				appendRegBPString(codeGenerator, topStack);
				appendString(codeGenerator, "]\n");

				appendString(codeGenerator, "	mov ecx,[");
				appendRegBPString(codeGenerator, R);
				appendString(codeGenerator, "]\n");

				genCodeTwoNodeOperation(codeGenerator, operator);

				appendString(codeGenerator, "	mov [");
				appendRegBPString(codeGenerator, R);
				appendString(codeGenerator, "],eax\n");

				safePush(tempStack, R);
				safeSwap(tempStack);
			} else {
				genCodeExpressionToEAX(symbolTable, codeGenerator, leftChild, tempStack);
				int topStack = safePeak(tempStack);
				if (treeGetSize(leftChild) < 2) {
					appendString(codeGenerator, "\n	mov [");
					appendRegBPString(codeGenerator, topStack);
					appendString(codeGenerator, "],eax");
				}
				int R = safePop(tempStack);
				genCodeExpressionToEAX(symbolTable, codeGenerator, rightChild, tempStack);
				topStack = safePeak(tempStack);
				
				appendString(codeGenerator, "\n	mov ecx,[");
				appendRegBPString(codeGenerator, topStack);
				appendString(codeGenerator, "]\n");

				appendString(codeGenerator, "	mov ebx,[");
				appendRegBPString(codeGenerator, R);
				appendString(codeGenerator, "]\n");

				genCodeTwoNodeOperation(codeGenerator, operator);

				appendString(codeGenerator, "	mov [");
				appendRegBPString(codeGenerator, R);
				appendString(codeGenerator, "],eax\n");

				safePush(tempStack, R);
			}
		}

	}
	return false;
}

static void genCodeTwoNodeOperation(CodeGenerator * codeGenerator, char * operator) {
	char * temp;
	if (strcmp(operator, "=") == 0) {
		temp = "\n	call _equal\n";
	} else if (strcmp(operator, "<>") == 0) {
		temp = "\n	call _not_equal\n";
	} else if (strcmp(operator, "<") == 0) {
		temp = "\n	call _less_than\n";
	} else if (strcmp(operator, "<=") == 0) {
		temp = "\n	call _less_than_equal\n";
	} else if (strcmp(operator, ">=") == 0) {
		temp = "\n	call _greater_than_equal\n";
	} else if (strcmp(operator, ">") == 0) {
		temp = "\n	call _greater_than\n";
	} else if (strcmp(operator, "+") == 0) {
		temp = "\n 	add ebx, ecx\n"
	    		"	mov eax, ebx\n";
	} else if (strcmp(operator, "-") == 0) {
		temp = "\n 	sub ebx, ecx\n"
	    		"	mov eax, ebx\n";
	} else if (strcmp(operator, "or") == 0) {
		temp = "\n 	or ebx, ecx\n"
	    		"	mov eax, ebx\n";
	} else if (strcmp(operator, "*") == 0) {
		temp =  "\n	mov eax, ebx\n"
				"	mul ecx\n";
	} else if (strcmp(operator, "/") == 0) {
		temp =  "\n	mov eax, ebx\n"
				"	mov edx, 0\n"
				"	div ecx\n";
	} else if (strcmp(operator, "div") == 0) {
		temp =  "\n	mov eax, ebx\n"
				"	mov edx, 0\n"
				"	div ecx\n";
	} else if (strcmp(operator, "mod") == 0) {
		temp =  "\n	mov eax, ebx\n"
				"	mov edx, 0\n"
				"	div ecx\n"
				"	mov eax,edx\n";
	} else if (strcmp(operator, "and") == 0) {
		temp = "\n	and ebx, ecx\n"
	    		"	mov eax, ebx\n";
	}
	appendString(codeGenerator, temp);
}


static int labelVariables(Tree * variableList, bool areParameters) {
	int numberOfVariables = parseTreeGetLabel(variableList);
	int labelIndex;
	
	if (areParameters)
		labelIndex = 2;
	else
		labelIndex = -2;

	if (numberOfVariables == 0)
		return labelIndex * 4;
	
	
	int listSize = treeGetSize(variableList) / 2;

	Iterator * iterator;
	if (areParameters)
		iterator = iteratorInitBack(treeGetChildren(variableList));
	else
		iterator = iteratorInit(treeGetChildren(variableList));

	for(int i = 0; i < listSize; i++) {

		Tree * identifier_list;
		Tree * typeTree; 
		if (areParameters) {
			typeTree = (Tree *) iteratorGetPrevious(iterator);
			identifier_list = (Tree *) iteratorGetPrevious(iterator);
		} else {
			identifier_list 	= (Tree *) iteratorGetNext(iterator);
			typeTree			= (Tree *) iteratorGetNext(iterator);
		}

		int type = parseTreeGetType(typeTree);

		int increment = 1;

		if( type == LL_ARRAY ) { // check array bounds
			increment ++;
			Tree * lowerBoundLocation = treeGetChild(typeTree, 0);
			Tree * upperBoundLocation = treeGetChild(typeTree, 1);
			int upperBound = parseTreeGetLabel(upperBoundLocation);
			int lowerBound = parseTreeGetLabel(lowerBoundLocation);
			increment += upperBound - lowerBound + 1;
		}

		int identifier_size = treeGetSize(identifier_list);
		Iterator * identifier_iterator;
		if (areParameters)
			identifier_iterator = iteratorInitBack(treeGetChildren(identifier_list));
		else
			identifier_iterator = iteratorInit(treeGetChildren(identifier_list));

		for(int j = 0; j < identifier_size; j++) {
			Tree * identifier;
			if(areParameters)
				identifier 	= (Tree *) iteratorGetPrevious(identifier_iterator);
			else
				identifier 	= (Tree *) iteratorGetNext(identifier_iterator);

			if (type == LL_ARRAY && areParameters) {
				labelIndex += increment;
				parseTreeSetLabel(identifier, (labelIndex-1) * 4);
			} else {
				parseTreeSetLabel(identifier, labelIndex * 4);
			
				if(areParameters)
					labelIndex += increment;
				else
					labelIndex -= increment;
			}
		}
		iteratorDestroy(identifier_iterator);
	}
	iteratorDestroy(iterator);

	return labelIndex * 4;
}


static void safeSwap(LinkedList * tempStack) {
	int i = safePop(tempStack);
	int j = safePop(tempStack);
	safePush(tempStack, i);
	safePush(tempStack, j);
}

static int safePop(LinkedList * tempStack) {
	int * temp = linkedListPeak(tempStack);
	int copy = *temp;
	linkedListPop(tempStack);
	return copy;
}

static void safePush(LinkedList * tempStack, int value) {
	int * temp = malloc(sizeof(int));
	*temp = value;
	linkedListPush(tempStack, temp);
}

static int safePeak(LinkedList * tempStack) {
	int * temp = linkedListPeak(tempStack);
	return *temp;
}

static void appendRegBPString(CodeGenerator * codeGenerator, int indexFromBP) {
	char * stringValue = intToString(indexFromBP);
	appendString(codeGenerator, "ebp");
	if (indexFromBP < 0) {
		appendString(codeGenerator, stringValue);
	} else {
		appendString(codeGenerator, "+");
		appendString(codeGenerator, stringValue);
	}
	free(stringValue);
}


static void appendString(CodeGenerator * codeGenerator, char * string) {
	linkedListPushBack(codeGenerator, copyString(string));
}

static void prependString(CodeGenerator * codeGenerator, char * string) {
	linkedListPush(codeGenerator, copyString(string));
}

#include <stdarg.h>
#include <string.h>
#include "code_generator.h"
#include "utils.h"
#include "iterator.h"
#include "linkedlist.h"
#include "hashtable.h"
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

static char * exitString = 
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


static char * readHeader =
"\nsection .bss \n"
"	read_buf          resb 50 \n"
"	read_buf_length   equ $ - read_buf \n"
" \n";

static char * readNumberP1 = 
"\n"
"_read: \n"
"	push ebp \n"
"	mov  ebp, esp \n"
" \n"
"	push 0 \n"
"_read_again: \n"
" \n";
static char * readNumberP2 =
" \n"
"	mov ecx, read_buf \n"
"	mov eax, 3             		; SYS_WRITE \n"
"	mov edx, read_buf_length    ; Amount of chars to print \n"
"	mov ebx, 1             		; STDOUT \n"
"	int 0x80 \n"
" \n"
"	mov eax,0 \n"
"	mov ebx,0 \n"
"	mov edx,1 \n"
"	mov [esp],edx \n"
"	mov bl,[ecx] \n"
"	cmp bl,'-' \n"
"	jne _read_translate_int \n"
"	mov edx,-1 \n"
"	mov [esp],edx \n"
"	add ecx,1 \n"
" \n"
"_read_translate_int: \n"
"	mov edx,ecx \n"
"	sub edx,read_buf \n"
"	cmp edx,read_buf_length \n"
"	je _read_check_done \n"
"	mov edx,[ecx] \n"
"	mov bl,dl \n"
"	cmp bl,10  ; new line detected \n"
"	je _read_check_done \n"
"	cmp bl,'0' \n"
"	jl _read_error \n"
"	cmp bl,'9' \n"
"	jg _read_error \n"
"	mov edx,10 \n"
"	mul edx \n"
"	sub bl,'0' \n"
"	add eax,ebx \n"
"	add ecx,1 \n"
"	jmp _read_translate_int \n"
"_read_check_done: \n"
"	cmp ecx,read_buf \n"
"	je _read_error \n"
"	sub ecx,1 \n"
"	mov edx,[ecx] \n"
"	cmp dl,'-' \n"
"	je _read_error \n"
"	mov edx,[esp] \n"
"	mul edx \n"
"	jmp _read_done ; read is done \n"
"_read_error: \n";

static char * readNumberP3 =
" \n"
"	jmp _read_again \n"
"_read_done: \n"
"	mov esp, ebp \n"
"	pop ebp \n"
"	ret \n";

static char * writeNumber = 
"\n_write: \n"
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
"	cmp eax, 0  \n"
"jne _write_store_digits \n"
" \n"
"	mov edx, 1              ; Amount of chars to print \n"
"	mov ebx, 1              ; STDOUT \n"
"_write_digits: \n"
"	mov ecx, esp \n"
"	mov eax, 4              ; SYS_WRITE \n"
"	int 0x80 \n"
" \n"
"	pop eax \n"
"	mov eax, ebp \n"
"	cmp eax, esp \n"
"jne _write_digits \n"
" \n"
"	mov esp, ebp \n"
"	pop ebp \n"
"	ret \n";


static char * writeNewLine = 
"\n"
"_write_new_line:\n"
"	mov eax, 0xa \n"
"	push eax \n"
"	mov ecx, esp \n"
"	mov eax, 4              ; SYS_WRITE \n"
"	mov edx, 1              ; Amount of chars to print \n"
"	mov ebx, 1              ; STDOUT \n"
"	int 0x80 \n"
"	pop eax \n"
"\n"
"	mov eax, 0xd \n"
"	push eax \n"
"	mov ecx, esp \n"
"	mov eax, 4              ; SYS_WRITE \n"
"	int 0x80 \n"
"	pop eax \n"
"	ret \n";


static char * lessThanCode =
"\n"
"_less_than:\n"
"	mov eax, -1\n"
"	cmp ebx, ecx  \n"
"	jl _less_than_jump\n"
"	mov eax, 0\n"
"_less_than_jump:\n"
"	ret\n"
"\n";

static char * lessThanEqualToCode =
"\n"
"_less_than_equal:\n"
"	mov eax, -1\n"
"	cmp ebx, ecx  \n"
"	jle _less_than_equal_jump\n"
"	mov eax, 0\n"
"_less_than_equal_jump:\n"
"	ret\n"
"\n";

static char * greaterThanCode =
"\n"
"_greater_than:\n"
"	mov eax, -1\n"
"	cmp ebx, ecx  \n"
"	jg _greater_than_jump\n"
"	mov eax, 0\n"
"_greater_than_jump:\n"
"	ret\n"
"\n";

static char * greaterThanEqualToCode =
"\n"
"_greater_than_equal:\n"
"	mov eax, -1\n"
"	cmp ebx, ecx  \n"
"	jge _greater_than_equal_jump\n"
"	mov eax, 0\n"
"_greater_than_equal_jump:\n"
"	ret\n"
"\n";

static char * equalToCode =
"\n"
"_equal:\n"
"	mov eax, -1\n"
"	cmp ebx, ecx  \n"
"	je _equal_jump\n"
"	mov eax, 0\n"
"_equal_jump:\n"
"	ret\n"
"\n";

static char * notEqualToCode =
"\n"
"_not_equal:\n"
"	mov eax, -1\n"
"	cmp ebx, ecx  \n"
"	jne _not_equal_jump\n"
"	mov eax, 0\n"
"_not_equal_jump:\n"
"	ret\n"
"\n";

static char * errorFunction =
"\n"
"_exit_with_error:	     \n"

"	mov	eax,1       ;system call number (sys_exit)\n"
"	mov	ebx,1       ; error 				\n"
"	int	0x80        ;call kernel\n";

static char * writeString =
"\n"
"_write_string:	     \n"
"	mov	ebx,1       	;file descriptor (stdout)\n"
"	mov	eax,4       	;system call number (sys_write)\n"
"	int	0x80        	;call kernel\n"
"	ret        		 \n";


static char * writeErrorString =
"\n"
"_write_error_string:	     \n"
"	mov	ebx,2       	;file descriptor (stderr)\n"
"	mov	eax,4       	;system call number (sys_write)\n"
"	int	0x80        	;call kernel\n"
"	ret        		 \n";


typedef struct CodeGenerator {
	FILE * fileToWrite;
	HashTable * dataStrings;
	bool indexArrays;
	bool usesLessThan;
	bool usesLessThanEqualTo;
	bool usesGreaterThan;
	bool usesGreaterThanEqualTo;
	bool usesEqualTo;
	bool usesNotEqualTo;
	bool usesWriteNewLine;
	bool usesWriteNumber;
	bool usesReadNumber;
	bool usesWriteString;
} CodeGenerator;

static char * messageLabel = "msg_";
static char * messageLengthLabel = "len_";

static ListPrintProperties callPrint = {"__", "_", ""};
static ListPrintProperties labelPrint = {"__", "_", "_"};
static ListPrintProperties tracePrintProperties = {"", ".", ""};

static CodeGenerator * initCodeGenerator(FILE * fileToWrite);
static void destroyCodeGenerator(CodeGenerator * codeGenerator);
static void appendString(CodeGenerator * codeGenerator, char * string);
static void appendManyStrings(CodeGenerator * codeGenerator, int numStrings, ...);
static Pair * getMessageLabelStrings(int label);
static void printCodeEnd(CodeGenerator * codeGenerator);

static void safeSwap(LinkedList * tempStack);
static int safePop(LinkedList * tempStack);
static void safePush(LinkedList * tempStack, int value);
static int safePeak(LinkedList * tempStack);
static void appendRegBPString(CodeGenerator * codeGenerator, int indexFromBP);
static void writePreMadeMessage(CodeGenerator * codeGenerator, char * message);
static void writePreMadeErrorMessage(CodeGenerator * codeGenerator, char * message);
static void pushPreMadeMessage(CodeGenerator * codeGenerator, char * message);
static void setPreMadeMessage(CodeGenerator * codeGenerator, char * message);
static void writeSetPreMadeMessage(CodeGenerator * codeGenerator);
static void popPreMadeErrorMessage(CodeGenerator * codeGenerator);

static int labelVariables(Tree * variableList, bool areParameters);

static void genCodeProgram(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * program);
static void genCodeSubPrograms(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * subPrograms);
static void genCodeProcedure(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * procedure);
static void genCodeFunction(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * function);
static void genCodeStatements(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * statements, LinkedList * tempStack);
static void genCodeStatement(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * statement, LinkedList * tempStack);
static void genCodeTwoNodeOperation(CodeGenerator * codeGenerator, char * operator);
static void genCodeReadProcedure(CodeGenerator * codeGenerator);
static void genCodeCheckArrayOutOfBounds(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * variable);
static void genCodeArrayIndexOutOfBoundsCall(CodeGenerator * codeGenerator);

static LinkedList * generateFullHeader(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * declarations, int lastRegisterUnused);
static void generateLocalVariables(CodeGenerator * codeGenerator, Tree * declarations);

static void getCallingVariablePutInEAX(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * variable);
static void callFunctionPutInEAX(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * functionCall, LinkedList * tempStack );
static void callProcedure(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * procedureCall, LinkedList * tempStack);
static void callWriteProcedure(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * procedureCall );
static void assignFromEAXToVariable(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * variable, LinkedList * tempStack, bool isReturnType);
static void genCodeIndexVariableAtEAX(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * variable);
static bool genCodeExpressionToEAX(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * expression, LinkedList * tempStack);
static int genCodePushArrayVariable(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * arrayType, Tree * variable);


static CodeGenerator * initCodeGenerator(FILE * fileToWrite) {
	CodeGenerator * codeGenerator = malloc(sizeof(CodeGenerator));
	codeGenerator->fileToWrite = fileToWrite;
	codeGenerator->dataStrings = hashTableInit(TABLE_SIZE, hashpjw, &STRING_OBJECT, &INT_OBJECT);
	codeGenerator->indexArrays = false;
	codeGenerator->usesLessThan = false;
	codeGenerator->usesLessThanEqualTo = false;
	codeGenerator->usesGreaterThan = false;
	codeGenerator->usesGreaterThanEqualTo = false;
	codeGenerator->usesEqualTo = false;
	codeGenerator->usesNotEqualTo = false;
	codeGenerator->usesWriteNewLine = false;
	codeGenerator->usesWriteNumber = false;
	codeGenerator->usesReadNumber = false;
	codeGenerator->usesWriteString = false;
	return codeGenerator;
}

static void destroyCodeGenerator(CodeGenerator * codeGenerator) {
	hashTableDestroy(codeGenerator->dataStrings);
	free(codeGenerator);
}


static void appendString(CodeGenerator * codeGenerator, char * string) {
	fputs(string, codeGenerator->fileToWrite);
}

static void appendManyStrings(CodeGenerator * codeGenerator, int numStrings, ...) {
	va_list   argList;
	va_start( argList, numStrings);
	for( int i = 0; i < numStrings; i++ )
    	appendString(codeGenerator, va_arg( argList, char * ));
    va_end( argList );
}


static void printCodeEnd(CodeGenerator * codeGenerator) {
	if (codeGenerator->usesLessThan)
		appendString(codeGenerator, lessThanCode);
	if (codeGenerator->usesLessThanEqualTo)
		appendString(codeGenerator, lessThanEqualToCode);
	if (codeGenerator->usesGreaterThan)
		appendString(codeGenerator, greaterThanCode);
	if (codeGenerator->usesGreaterThanEqualTo)
		appendString(codeGenerator, greaterThanEqualToCode);
	if (codeGenerator->usesEqualTo)
		appendString(codeGenerator, equalToCode);
	if (codeGenerator->usesNotEqualTo)
		appendString(codeGenerator, notEqualToCode);
	if (codeGenerator->usesWriteNumber || codeGenerator->indexArrays)
		appendString(codeGenerator, writeNumber);
	if (codeGenerator->usesReadNumber)
		genCodeReadProcedure(codeGenerator);
	if (codeGenerator->usesWriteNewLine)
		appendString(codeGenerator, writeNewLine);
	if (codeGenerator->usesWriteString)
		appendString(codeGenerator, writeString);
	if (codeGenerator->indexArrays){
		genCodeArrayIndexOutOfBoundsCall(codeGenerator);
		appendString(codeGenerator, writeErrorString);
		appendString(codeGenerator, errorFunction);
	}
	
	if (codeGenerator->usesReadNumber)
		appendString(codeGenerator, readHeader);

	if (hashTableSize(codeGenerator->dataStrings) > 0) {
		appendString(codeGenerator, "\n\nsection	.data\n");

		Iterator * stringPairIterator = hashTableIteratorInit(codeGenerator->dataStrings);
		while (iteratorHasNext(stringPairIterator)) {
			Pair * pair = iteratorGetNext(stringPairIterator);
			char * message = pair->first;
			int * messageNumberPtr = pair->second;
			Pair * stringLabelPair = getMessageLabelStrings(*messageNumberPtr);
			char * msgString = stringLabelPair->first;
			char * lenString = stringLabelPair->second;
			appendManyStrings(codeGenerator, 3, "\n	", msgString, " db ");

			int startIndex = 0;
			int i = 0;
			char c = message[0];
			while (c != '\0') {
				if (c == '\n') {
					if (startIndex != 0)
						appendString(codeGenerator, ", ");
					if (i != startIndex) {
						char * substring = stringTakeSubstring(message, startIndex, i);
						appendManyStrings(codeGenerator, 3, "'", substring,"', ");
						free(substring);						
					}
					appendString(codeGenerator, "10, 13");
					startIndex = i + 1;
				}
				c = message[++i];
			}
			
			if (i != startIndex) {
				if (startIndex != 0)
					appendString(codeGenerator, ", ");
				char * substring = stringTakeSubstring(message, startIndex, i);
				appendManyStrings(codeGenerator, 3, "'", substring,"'\n");
				free(substring);						
			} else {
				appendString(codeGenerator, "\n");
			}

			appendManyStrings(codeGenerator, 5, "	", lenString, " equ $ - ", msgString, "\n");
			
			free(lenString);
			free(msgString);
			free(stringLabelPair);
		}
		iteratorDestroy(stringPairIterator);
	}
}

static void genCodeReadProcedure(CodeGenerator * codeGenerator) {
	appendString(codeGenerator, readNumberP1);
	writeSetPreMadeMessage(codeGenerator);
	writePreMadeMessage(codeGenerator, " = ");
	appendString(codeGenerator, readNumberP2);
	writePreMadeMessage(codeGenerator, "[Error] Please enter a valid integer\n");
	appendString(codeGenerator, readNumberP3);
}

static Pair * getMessageLabelStrings(int label) {
	char * labelString = intToString(label);
	int labelStringLength = getStringSize(labelString);
	int msgStringLength = getStringSize(messageLabel);
	int lenStringLength = getStringSize(messageLengthLabel);
	int msgTotalLength = msgStringLength + labelStringLength;
	int lenTotalLength = lenStringLength + labelStringLength;

	char * msgString = malloc(sizeof(char) * (msgTotalLength + 1)); 
	char * lenString = malloc(sizeof(char) * (lenTotalLength + 1));

	stringInsert(msgString, messageLabel, 0);
	stringInsert(msgString, labelString, msgStringLength);

	stringInsert(lenString, messageLengthLabel, 0);
	stringInsert(lenString, labelString, lenStringLength);

	msgString[msgTotalLength] = '\0';
	lenString[lenTotalLength] = '\0';

	Pair * pair = malloc(sizeof(Pair));
	pair->first = msgString;
	pair->second = lenString;
	free(labelString);

	return pair;
}


static void writePreMadeMessage(CodeGenerator * codeGenerator, char * message) {
	int * messageNumberPtr = hashTableGet(codeGenerator->dataStrings, message);
	if (messageNumberPtr == 0) {
		messageNumberPtr = malloc(sizeof(int));
		*messageNumberPtr = hashTableSize(codeGenerator->dataStrings);
		hashTablePut(codeGenerator->dataStrings, copyString(message), messageNumberPtr);
	}
	Pair * stringPair = getMessageLabelStrings(*messageNumberPtr);

	char * msgString = stringPair->first; 
	char * lenString = stringPair->second;

	appendManyStrings(codeGenerator, 2, "\n	mov ecx,", msgString);
	appendManyStrings(codeGenerator, 2, "\n	mov edx,", lenString);
	appendString(codeGenerator, "\n	call _write_string\n");

	free(lenString);
	free(msgString);
	free(stringPair);
}

static void writePreMadeErrorMessage(CodeGenerator * codeGenerator, char * message) {
	int * messageNumberPtr = hashTableGet(codeGenerator->dataStrings, message);
	if (messageNumberPtr == 0) {
		messageNumberPtr = malloc(sizeof(int));
		*messageNumberPtr = hashTableSize(codeGenerator->dataStrings);
		hashTablePut(codeGenerator->dataStrings, copyString(message), messageNumberPtr);
	}
	Pair * stringPair = getMessageLabelStrings(*messageNumberPtr);

	char * msgString = stringPair->first; 
	char * lenString = stringPair->second;

	appendManyStrings(codeGenerator, 2, "\n	mov ecx,", msgString);
	appendManyStrings(codeGenerator, 2, "\n	mov edx,", lenString);
	appendString(codeGenerator, "\n	call _write_error_string\n");

	free(lenString);
	free(msgString);
	free(stringPair);
}

static void pushPreMadeMessage(CodeGenerator * codeGenerator, char * message) {
	int * messageNumberPtr = hashTableGet(codeGenerator->dataStrings, message);
	if (messageNumberPtr == 0) {
		messageNumberPtr = malloc(sizeof(int));
		*messageNumberPtr = hashTableSize(codeGenerator->dataStrings);
		hashTablePut(codeGenerator->dataStrings, copyString(message), messageNumberPtr);
	}
	Pair * stringPair = getMessageLabelStrings(*messageNumberPtr);

	char * msgString = stringPair->first; 
	char * lenString = stringPair->second;

	appendManyStrings(codeGenerator, 3, "	push ", lenString, "\n");
	appendManyStrings(codeGenerator, 3, "	push ", msgString, "\n");

	free(lenString);
	free(msgString);
	free(stringPair);
}

static void popPreMadeErrorMessage(CodeGenerator * codeGenerator) {
	appendString(codeGenerator, "\n	pop ecx\n	pop edx");
	appendString(codeGenerator, "\n	call _write_error_string\n");
}


static void setPreMadeMessage(CodeGenerator * codeGenerator, char * message) {
	int * messageNumberPtr = hashTableGet(codeGenerator->dataStrings, message);
	if (messageNumberPtr == 0) {
		messageNumberPtr = malloc(sizeof(int));
		*messageNumberPtr = hashTableSize(codeGenerator->dataStrings);
		hashTablePut(codeGenerator->dataStrings, copyString(message), messageNumberPtr);
	}
	Pair * stringPair = getMessageLabelStrings(*messageNumberPtr);

	char * msgString = stringPair->first; 
	char * lenString = stringPair->second;

	appendManyStrings(codeGenerator, 3, "	mov eax,", lenString, "\n	mov [esp+4],eax\n");
	appendManyStrings(codeGenerator, 3, "	mov eax,", msgString, "\n	mov [esp],eax\n");

	free(lenString);
	free(msgString);
	free(stringPair);
}

static void writeSetPreMadeMessage(CodeGenerator * codeGenerator) {
	appendString(codeGenerator, "\n	mov ecx,[ebp+8]\n	mov edx,[ebp+12]");
	appendString(codeGenerator, "\n	call _write_string\n");
}

void generateCode(FILE * fileToWrite, Tree * tree, SymbolTable * symbolTable) {
	CodeGenerator * codeGenerator = initCodeGenerator(fileToWrite);
	char * programName = parseTreeGetAttribute(tree);
	char * callLabel = symbolTableScopeTraceString(symbolTable, &callPrint);

	appendString(codeGenerator, startString);
	appendManyStrings(codeGenerator, 4, "	call ", callLabel, programName, "\n");
	appendString(codeGenerator, exitString);
	
	genCodeProgram(symbolTable, codeGenerator, tree);

	printCodeEnd(codeGenerator);

	free(callLabel);
	destroyCodeGenerator(codeGenerator);
}



static LinkedList * generateFullHeader(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * declarations, int lastRegisterUnused) {
	char * callLabel = symbolTableScopeTraceString(symbolTable, &callPrint);
	appendManyStrings(codeGenerator, 3, "\n", callLabel, ":\n");
	appendString(codeGenerator, header);

	appendString(codeGenerator, "	push eax	; base pointer of parent \n");

	generateLocalVariables(codeGenerator, declarations);

	int numberOfTempRegs = symbolTablegetNumTempRegs(symbolTable);
	int lastLastRegisterUsed = lastRegisterUnused - 4 * numberOfTempRegs;

	LinkedList * tempStack = linkedListInit(&INT_OBJECT);
	for (int i = lastRegisterUnused; i > lastLastRegisterUsed; i-= 4) {
		safePush(tempStack, i);
		appendString(codeGenerator, "	push 0		; temporary register \n");	
	}

	free(callLabel);

	return tempStack;
}

static void generateLocalVariables(CodeGenerator * codeGenerator, Tree * declarations) {
	Iterator * typeIterator = linkedListIteratorInit(treeGetChildren(declarations));
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

			Iterator * iterator = linkedListIteratorInit(treeGetChildren(identifier_list));
			while (iteratorHasNext(iterator)) {
				Tree * variable = iteratorGetNext(iterator);
				appendManyStrings(codeGenerator, 3, "	push ", lowerBoundString,  "		; lower bound array\n");
				appendManyStrings(codeGenerator, 3, "	push ", upperBoundString,  "		; upper bound array\n");
				for(int i = lowerBound; i <= upperBound; i++)
					appendString(codeGenerator, "	push 0		; declared array element \n");
			}
			free(lowerBoundString);
			free(upperBoundString);
			iteratorDestroy(iterator);
		} else {
			Iterator * iterator = linkedListIteratorInit(treeGetChildren(identifier_list));
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
	Iterator * iterator = linkedListIteratorInit(treeGetChildren(subPrograms));
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

	appendManyStrings(codeGenerator, 3, "\n", function_end, "end:\n");
	appendString(codeGenerator, footer);

	free(function_end);
	linkedListDestroy(tempStack);
	symbolTablePopScope(symbolTable);
}

static void genCodeStatements(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * statements, LinkedList * tempStack) {
	int numberOfStatements = parseTreeGetLabel(statements);

	if(numberOfStatements == 0)
		return;
	Iterator * iterator = linkedListIteratorInit(treeGetChildren(statements));
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
				appendManyStrings(codeGenerator, 3, "\n	jmp ", function_end, "end\n");
				free(function_end);
			}
			break;
		case READ_PROCEDURE_ID:
			;
			codeGenerator->usesReadNumber = true;
			codeGenerator->usesWriteString = true;
			Tree * read_argumentList = treeGetChild(statement, 0);
			Iterator * read_arguments_iterator = linkedListIteratorInit(treeGetChildren(read_argumentList));
			appendString(codeGenerator, "\n	push 0\n	push 0\n");
			while (iteratorHasNext(read_arguments_iterator)) {
				Tree * arg = iteratorGetNext(read_arguments_iterator);
				char * variableName = parseTreeGetAttribute(arg);
				setPreMadeMessage(codeGenerator, variableName);
				appendString(codeGenerator, "	call _read\n");
				assignFromEAXToVariable(symbolTable, codeGenerator, arg, tempStack, false);
			}
			appendString(codeGenerator, "\n	pop eax\n	pop eax\n");
			iteratorDestroy(read_arguments_iterator);
			break;
		case WRITE_PROCEDURE_ID:
			;
			codeGenerator->usesWriteNewLine = true;
			codeGenerator->usesWriteNumber = true;
			Tree * write_argumentList = treeGetChild(statement, 0);
			Iterator * write_arguments_iterator = linkedListIteratorInit(treeGetChildren(write_argumentList));
			appendString(codeGenerator, "\n	push 0\n");
			while (iteratorHasNext(write_arguments_iterator)) {
				Tree * arg = iteratorGetNext(write_arguments_iterator);
				genCodeExpressionToEAX(symbolTable, codeGenerator, arg, tempStack);
				appendString(codeGenerator, "\n	mov [esp],eax\n");
				appendString(codeGenerator, "	call _write\n");
				appendString(codeGenerator, "	call _write_new_line");
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
			char * if_label = symbolTableScopeTraceString(symbolTable, &labelPrint);

			genCodeExpressionToEAX(symbolTable, codeGenerator, if_condition, tempStack);
			appendString(codeGenerator, "\n	cmp eax, 0\n");
			appendManyStrings(codeGenerator, 5, "\n	je ", if_label, "else_", if_label_string, "\n");

			genCodeStatement(symbolTable, codeGenerator,  treeGetChild(statement, 1), tempStack);
			if (treeGetSize(statement) == 2) { // if then
				appendManyStrings(codeGenerator, 5, "\n", if_label, "else_", if_label_string, ":\n");
			} else { // if then else
				appendManyStrings(codeGenerator, 5, "\n	jmp ", if_label, "end_if_", if_label_string, "\n");
				appendManyStrings(codeGenerator, 4, if_label, "else_", if_label_string, ":\n");
				genCodeStatement(symbolTable, codeGenerator,  treeGetChild(statement, 2), tempStack);
				appendManyStrings(codeGenerator, 5, "\n", if_label, "end_if_", if_label_string, ":\n");
			}
			free(if_label);
			free(if_label_string);
			break;
		case LL_WHILE:
			;
			Tree * while_condition = treeGetChild(statement, 0);
			char *  while_label_string = intToString( parseTreeGetLabel(statement));
			char * while_label = symbolTableScopeTraceString(symbolTable, &labelPrint);
			appendManyStrings(codeGenerator, 5, "\n", while_label, "while_", while_label_string, ":\n");
			genCodeExpressionToEAX(symbolTable, codeGenerator, while_condition, tempStack);
			appendString(codeGenerator, "\n	cmp eax, 0\n");
			appendManyStrings(codeGenerator, 5, "\n	je ", while_label, "while_end_",  while_label_string, "\n");
			genCodeStatement(symbolTable, codeGenerator, treeGetChild(statement, 1), tempStack);
			appendManyStrings(codeGenerator, 5, "\n	jmp ", while_label, "while_", while_label_string, "\n");
			appendManyStrings(codeGenerator, 4, while_label, "while_end_", while_label_string, ":\n");
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
			appendString(codeGenerator, "\n	mov eax,1\n	mov ebx,[esp+4]\n	mov ecx,[esp]\n	cmp ebx,ecx\n");
			appendManyStrings(codeGenerator, 5, "	jle ", for_label, "for_incr_", for_label_string, "\n");
			appendString(codeGenerator, "\n	mov eax,-1\n");
			appendManyStrings(codeGenerator, 4, for_label, "for_incr_", for_label_string, ":\n");
			appendString(codeGenerator, "\n	mov [esp+4],eax\n");
			appendManyStrings(codeGenerator, 4, for_label, "for_", for_label_string, ":\n");
			genCodeStatement(symbolTable, codeGenerator, forBody, tempStack);
			
			getCallingVariablePutInEAX(symbolTable, codeGenerator, iteratorVariable);
			appendString(codeGenerator, "\n	mov ebx,[esp]\n 	cmp eax,ebx\n");
			appendManyStrings(codeGenerator, 5, "	je ", for_label, "for_end_", for_label_string, "\n");
			
			appendString(codeGenerator, "\n	mov ebx,[esp+4]\n 	add eax,ebx\n");
			assignFromEAXToVariable(symbolTable, codeGenerator, iteratorVariable, tempStack, false);

			appendManyStrings(codeGenerator, 5, "\n	jmp ", for_label, "for_", for_label_string, "\n");
			appendManyStrings(codeGenerator, 4, for_label, "for_end_", for_label_string, ":\n");
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
	char * variableOffsetString = intToString(variableOffset);
	appendManyStrings(codeGenerator, 3, "\n	add ebx,", variableOffsetString, "\n");
	appendString(codeGenerator, "	mov eax,[ebx]\n");
	free(variableOffsetString);
}

static void assignFromEAXToVariable(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * variable, LinkedList * tempStack, bool isReturnType) {
	appendString(codeGenerator, "\n	push eax\n");

	if ( ! treeIsLeaf(variable) ) {
		codeGenerator->indexArrays = true;
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
	char * variableOffsetString = intToString(variableOffset);
	appendManyStrings(codeGenerator, 3, "	add ebx,", variableOffsetString, "\n");

	if ( ! treeIsLeaf(variable) ) {
		genCodeCheckArrayOutOfBounds(symbolTable, codeGenerator, variable);
		appendString(codeGenerator, "	mov eax,[ebx]\n	sub edx,eax\n");
		appendString(codeGenerator, "	sub ebx,8\n	mov eax,edx \n	mov edx,4 \n	mul edx\n	 sub ebx,eax\n");
	}
	appendString(codeGenerator, "	pop eax\n	mov [ebx],eax \n");

	free(variableOffsetString);
}

static void genCodeIndexVariableAtEAX(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * variable) { // indexing arrays

	appendString(codeGenerator, "\n	mov edx,eax\n");

	codeGenerator->indexArrays = true;
	char * variableName = parseTreeGetAttribute(variable);
	SearchResult searchResult;
	symbolTableSearchAll(symbolTable, variableName, &searchResult);
	int i = searchResult.searchDepth;

	appendString(codeGenerator, "	mov ebx,ebp\n");
	for(int j = 0; j < i; j++)
		appendString(codeGenerator, "	mov ecx,[ebx-4]\n	mov ebx,ecx\n");
	Tree * variableType = searchResult.attributes->variableValue;
	int variableOffset = parseTreeGetLabel(variableType);
	char * variableOffsetString = intToString(variableOffset);
	appendManyStrings(codeGenerator, 3, "	add ebx,", variableOffsetString, "\n");

	genCodeCheckArrayOutOfBounds(symbolTable, codeGenerator, variable);
	
	appendString(codeGenerator, "	mov eax,[ebx]\n	sub edx,eax\n");
	appendString(codeGenerator, "	sub ebx,8\n	mov eax,edx \n	mov edx,4 \n	mul edx\n	 sub ebx,eax\n");

	appendString(codeGenerator, "	mov eax,[ebx] \n");

	free(variableOffsetString);
}


static void genCodeCheckArrayOutOfBounds(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * variable) {
	int numberOfIndexedArrays = symbolTableGetNumIndexedArrays(symbolTable);
	char * variableName = parseTreeGetAttribute(variable);
	char * indexLabelString = intToString(numberOfIndexedArrays);
	char * indexedTraceString = symbolTableScopeTraceString(symbolTable, &labelPrint);
	char * programTrace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
	int lineDeclared = parseTreeGetLineNumberDeclared(variable);
	int lineIndexDeclared = parseTreeGetLineIndexDeclared(variable);
	char * lineDeclaredString = intToString(lineDeclared);
	char * lineIndexDeclaredString = intToString(lineIndexDeclared);

	appendString(codeGenerator, "	mov eax,[ebx]\n");
	appendString(codeGenerator, "	cmp edx,eax\n");
	appendManyStrings(codeGenerator, 5, "	jge ", indexedTraceString, "if_lowerbound_okay_", indexLabelString, "\n");

	appendString(codeGenerator, "	mov ecx,-1\n");
	appendManyStrings(codeGenerator, 4, indexedTraceString, "if_out_of_bounds_", indexLabelString, ":\n");
	pushPreMadeMessage(codeGenerator, programTrace);
	pushPreMadeMessage(codeGenerator, lineIndexDeclaredString);
	pushPreMadeMessage(codeGenerator, lineDeclaredString);
	pushPreMadeMessage(codeGenerator, variableName);
	appendString(codeGenerator, "	push ecx\n	jmp _array_out_of_bounds\n");
	appendManyStrings(codeGenerator, 4, indexedTraceString, "if_lowerbound_okay_", indexLabelString, ":\n");
	appendString(codeGenerator, "	mov eax,[ebx-4]\n");
	appendString(codeGenerator, "	cmp edx,eax\n");
	appendString(codeGenerator, "	mov ecx,0\n");
	appendManyStrings(codeGenerator, 5, "	jg ", indexedTraceString, "if_out_of_bounds_", indexLabelString, "\n");

	symbolTableIncrIndexedArrays(symbolTable); 
}


static void genCodeArrayIndexOutOfBoundsCall(CodeGenerator * codeGenerator) {
	appendString(codeGenerator, "\n_array_out_of_bounds:\n");


	appendString(codeGenerator, "\n	push eax\n	push edx\n");
	writePreMadeErrorMessage(codeGenerator, "[ERROR] ARRAY \"");

	appendString(codeGenerator, "\n	mov ecx,[esp+12]\n	mov edx,[esp+16]");
	appendString(codeGenerator, "\n	call _write_error_string\n");
	
	writePreMadeErrorMessage(codeGenerator, "\" is indexed at ");
	appendString(codeGenerator, "\n	call _write\n");
	appendString(codeGenerator, "\n	mov eax, [esp+8]\n	cmp eax,0\n	je _array_out_of_bounds_upper\n");
	writePreMadeErrorMessage(codeGenerator, " which is less than the lower bound of ");
	appendString(codeGenerator, "	jmp _array_out_of_bounds_lower\n_array_out_of_bounds_upper:\n");
	writePreMadeErrorMessage(codeGenerator, " which is greater than the upper bound of ");
	appendString(codeGenerator, "_array_out_of_bounds_lower:\n");
	appendString(codeGenerator, "\n	pop edx\n	call _write\n");
	for (int i = 0; i < 4; i++)
		appendString(codeGenerator, "	pop edx\n");

	writePreMadeErrorMessage(codeGenerator, " at (");
	popPreMadeErrorMessage(codeGenerator);
	writePreMadeErrorMessage(codeGenerator, ",");
	popPreMadeErrorMessage(codeGenerator);
	writePreMadeErrorMessage(codeGenerator, ") [TRACE: ");
	popPreMadeErrorMessage(codeGenerator);
	writePreMadeErrorMessage(codeGenerator, "]\n");
	appendString(codeGenerator, "\n	call _exit_with_error\n");
}

static int genCodePushArrayVariable(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * arrayType, Tree * variable) { // whole arrays
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
	char * variableOffsetString = intToString(variableOffset);
	appendManyStrings(codeGenerator, 3, "	add ebx,", variableOffsetString, "\n");

	appendString(codeGenerator, "	mov eax,[ebx]\n	push eax\n	mov eax,[ebx-4]\n	push eax\n");

	for(int i = 0; i < arraySize; i++) {
		char * indexValue = intToString(i * 4 + 8);
		appendManyStrings(codeGenerator, 3, "	mov eax,[ebx-", indexValue, "]\n");
		appendString(codeGenerator, "	push eax\n");
		free(indexValue);
	}

	free(variableOffsetString);
	return arraySize + 2;
}


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

	appendString(codeGenerator, "\n	push 0\n"); // push return value
	int numberOfPushedElements = 1;
	if (treeGetSize(functionCall) > 0) {
		Tree * parameterList = treeGetChild(procedureDefinition, 0);
		Iterator * parameterListIterator = linkedListIteratorInit(treeGetChildren(parameterList));

		Tree * write_argumentList = treeGetChild(functionCall, 0);
		Iterator * write_arguments_iterator = linkedListIteratorInit(treeGetChildren(write_argumentList));

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
	}

	appendString(codeGenerator, "\n	mov eax, ebp\n");
	for (int i = 0; i < searchDepth; i++)
		appendString(codeGenerator, "	mov ecx,[eax-4]\n	mov eax,ecx\n");
	char * callLabel = symbolTableScopeTraceStringAtDepth(symbolTable, searchDepth, &labelPrint);
	appendManyStrings(codeGenerator, 4, "\n	call ", callLabel, functionName, "\n");
	for (int i = 0; i < numberOfPushedElements; i++)
		appendString(codeGenerator, "	pop eax\n");

	free(callLabel);	
}

static void callProcedure(SymbolTable * symbolTable, CodeGenerator * codeGenerator, Tree * procedureCall, LinkedList * tempStack) {
	char * procedureName = parseTreeGetAttribute(procedureCall);

	SearchResult searchResult;
	symbolTableSearchAll(symbolTable, procedureName, &searchResult);

	int numberOfPushedElements = 0;
	if (treeGetSize(procedureCall) > 0) {
		Tree * procedureDefinition = searchResult.attributes->variableValue;
		Tree * parameterList = treeGetChild(procedureDefinition, 0);
		Iterator * parameterListIterator = linkedListIteratorInit(treeGetChildren(parameterList));


		Tree * write_argumentList = treeGetChild(procedureCall, 0);
		Iterator * write_arguments_iterator = linkedListIteratorInit(treeGetChildren(write_argumentList));

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
	}

	int searchDepth = searchResult.searchDepth;
	appendString(codeGenerator, "\n	mov eax, ebp\n");
	for (int i = 0; i < searchDepth; i++)
		appendString(codeGenerator, "	mov ecx,[eax-4]\n	mov eax,ecx\n");
	char * callLabel = symbolTableScopeTraceStringAtDepth(symbolTable, searchDepth, &labelPrint);
	appendManyStrings(codeGenerator, 4, "\n	call ", callLabel, procedureName, "\n");
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
		appendManyStrings(codeGenerator, 3, "\n	mov eax,", i2, "\n");
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
		codeGenerator->usesEqualTo = true;
		temp = "\n	call _equal\n";
	} else if (strcmp(operator, "<>") == 0) {
		codeGenerator->usesNotEqualTo = true;
		temp = "\n	call _not_equal\n";
	} else if (strcmp(operator, "<") == 0) {
		codeGenerator->usesLessThan = true;
		temp = "\n	call _less_than\n";
	} else if (strcmp(operator, "<=") == 0) {
		codeGenerator->usesLessThanEqualTo = true;
		temp = "\n	call _less_than_equal\n";
	} else if (strcmp(operator, ">=") == 0) {
		codeGenerator->usesGreaterThanEqualTo = true;
		temp = "\n	call _greater_than_equal\n";
	} else if (strcmp(operator, ">") == 0) {
		codeGenerator->usesGreaterThan = true;
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
		iterator = linkedListIteratorInitBack(treeGetChildren(variableList));
	else
		iterator = linkedListIteratorInit(treeGetChildren(variableList));

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
			identifier_iterator = linkedListIteratorInitBack(treeGetChildren(identifier_list));
		else
			identifier_iterator = linkedListIteratorInit(treeGetChildren(identifier_list));

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
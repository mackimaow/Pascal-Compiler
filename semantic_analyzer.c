#include <stdbool.h>
#include <string.h>
#include "semantic_analyzer.h"
#include "iterator.h"
#include "linkedlist.h"
#include "lexconstants.h"
#include "utils.h"
#include "tree.h"
#include "parsetree.h"



// things that may change for gencode:
static int ARRAY_TYPE_ARGUMENT_LABEL = 0;
static int FUNCTION_WITH_NO_PARAMETERS = 0;
static int SINGLE_NODE_EXPRESSION = 0;




// Misc
static ListPrintProperties tracePrintProperties = {"", ".", ""};
static bool LEFT_SIDE = true;
static bool RIGHT_SIDE = false;


// utility functions

static void addVariables(SymbolTable * symbolTable, Tree * variableLocations);
static void addSubPrograms(SymbolTable * symbolTable, Tree * programLocations);
static int checkSubProgramName( SymbolTable * symbolTable, Tree * statementCall, int type );

// implemented functions

static void checkProgram(SymbolTable * symbolTable, Tree * program);
static void checkSubPrograms(SymbolTable * symbolTable, Tree * programLocations);
static void checkProcedure(SymbolTable * symbolTable, Tree * procedure);
static void checkFunction(SymbolTable * symbolTable, Tree * function);
static bool checkStatements(SymbolTable * symbolTable, Tree * statements);
static bool checkStatement(SymbolTable * symbolTable, Tree * statement);
static bool checkAssignment(SymbolTable * symbolTable, Tree * assignmentStatement);
static int checkExpression(SymbolTable * symbolTable, Tree * expression);
static void checkConditionIsBoolean (SymbolTable * symbolTable, Tree * condition);
static void checkExpressionIsVariableType(SymbolTable * symbolTable, Tree * expression, Tree * variable, int variableType );
static int checkVariableName(SymbolTable * symbolTable, Tree * variable, bool sideCalled, Tree * arrayType); // arrayType is optional and is used if whole array is expected

// implementation

SymbolTable * analyzeSemantics(Tree * tree) {
	SymbolTable * symbolTable = symbolTableInit();
	checkProgram(symbolTable, tree);
	return symbolTable;
}



static void checkProgram(SymbolTable * symbolTable, Tree * program) {
	symbolTableCreateScope(symbolTable, program);
	char * program_name = parseTreeGetAttribute(program);
	symbolTablePushScope(symbolTable, program_name);

	Tree * parameters = treeGetChild(program, 0); // skip 
	Tree * declarations = treeGetChild(program, 1);
	Tree * subprogram_declarations = treeGetChild(program, 2);
	Tree * statement_list = treeGetChild(program, 3);

	addVariables(symbolTable, declarations);
	addSubPrograms(symbolTable, subprogram_declarations);
	checkSubPrograms(symbolTable, subprogram_declarations);
	checkStatements(symbolTable, statement_list);
	symbolTablePopScope(symbolTable);
}



static void checkSubPrograms(SymbolTable * symbolTable, Tree * programLocations) {
	if(!parseTreeIsNull(programLocations)) {
		Iterator * iterator = linkedListIteratorInit(treeGetChildren(programLocations));
		while(iteratorHasNext(iterator)) {
			Tree * subProgram = (Tree *) iteratorGetNext(iterator);
			int type = parseTreeGetType(subProgram);
			bool isFunction = type == LL_FUNCTION;
			if(isFunction)
				checkFunction(symbolTable, subProgram);
			else
				checkProcedure(symbolTable, subProgram);
		}
		iteratorDestroy(iterator);
	}
}

static void checkProcedure(SymbolTable * symbolTable, Tree * procedure) {
	symbolTableCreateScope(symbolTable, procedure);
	char * procedure_name = parseTreeGetAttribute(procedure);
	symbolTablePushScope(symbolTable, procedure_name);

	Tree * parameters = treeGetChild(procedure, 0); 
	Tree * declarations = treeGetChild(procedure, 1);
	Tree * subprogram_declarations = treeGetChild(procedure, 2);
	Tree * statement_list = treeGetChild(procedure, 3);


	addVariables(symbolTable, parameters);
	addVariables(symbolTable, declarations);
	addSubPrograms(symbolTable, subprogram_declarations);
	checkSubPrograms(symbolTable, subprogram_declarations);
	checkStatements(symbolTable, statement_list);
	symbolTablePopScope(symbolTable);
}

static void checkFunction(SymbolTable * symbolTable, Tree * function) {
	symbolTableCreateScope(symbolTable, function);
	char * function_name = parseTreeGetAttribute(function);
	symbolTablePushScope(symbolTable, function_name);

	Tree * parameters = treeGetChild(function, 0); 
	Tree * returnTypeTree = treeGetChild(function, 1); 
	Tree * declarations = treeGetChild(function, 2);
	Tree * subprogram_declarations = treeGetChild(function, 3);
	Tree * statement_list = treeGetChild(function, 4);

	symbolTablePut(symbolTable, function, returnTypeTree); // temporarly add function name to block declarations with same name
	addVariables(symbolTable, parameters);
	addVariables(symbolTable, declarations);
	symbolTableRemove(symbolTable, function_name); // temporarly add function name to block others declarations with same name [end]
	
	addSubPrograms(symbolTable, subprogram_declarations);
	checkSubPrograms(symbolTable, subprogram_declarations);
	bool returnedType = checkStatements(symbolTable, statement_list);
	if (!returnedType) { 
		int lineDeclared = parseTreeGetLineNumberDeclared(function);
		int lineIndexDeclared = parseTreeGetLineIndexDeclared(function);
		char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
		fprintf(stderr, "[ERROR] FUNCTION \"%s\" defined at (%i,%i) does not always return a value. [TRACE: %s]\n", 
			function_name, lineDeclared, lineIndexDeclared, trace);
		exit(1);
	}
	symbolTablePopScope(symbolTable);
}

static bool checkStatements(SymbolTable * symbolTable, Tree * statements){
	int numberOfStatements = 0;
	bool statementReturned = false;
	if(!parseTreeIsNull(statements)) {
		Iterator * iterator = linkedListIteratorInit(treeGetChildren(statements));
		while (iteratorHasNext(iterator)) {
			Tree * statement = (Tree *) iteratorGetNext(iterator);
			statementReturned = checkStatement(symbolTable, statement);
			if (statementReturned && iteratorHasNext(iterator)) {
				statement = iteratorGetNext(iterator);
				int lineDeclared = parseTreeGetLineNumberDeclared(statement);
				int lineIndexDeclared = parseTreeGetLineIndexDeclared(statement);
				char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
				fprintf(stderr, "[ERROR] DEAD CODE (%i,%i). [TRACE: %s]\n", 
					lineDeclared, lineIndexDeclared, trace);
				exit(1);
			}
			numberOfStatements++;
		}
		iteratorDestroy(iterator);
	}
	parseTreeSetLabel(statements, numberOfStatements);
	return statementReturned;
}


static bool checkStatement(SymbolTable * symbolTable, Tree * statement) {
	int type = parseTreeGetType(statement);

	switch (type) {
		case LL_ASSIGNOP:
			;
			return checkAssignment(symbolTable, statement);
		case LL_ID: // procedure call
			checkSubProgramName( symbolTable, statement, LL_PROCEDURE );
			return false;
		case LL_IF:
			parseTreeSetLabel(statement, symbolTableGetNumIf(symbolTable));
			symbolTableIncrIf(symbolTable);

			Tree * if_condition = treeGetChild(statement, 0);
			checkConditionIsBoolean(symbolTable, if_condition );
			
			bool firstStatement = checkStatement(symbolTable, treeGetChild(statement, 1));
			bool secondStatement = false; 
			if(treeGetSize(statement) == 3 )
				secondStatement = checkStatement(symbolTable, treeGetChild(statement, 2));
			return firstStatement && secondStatement; 
		case LL_WHILE:
			parseTreeSetLabel(statement, symbolTableGetNumWhile(symbolTable));
			symbolTableIncrWhile(symbolTable);
			
			Tree * while_condition = treeGetChild(statement, 0);
			checkConditionIsBoolean(symbolTable, while_condition );
			
			checkStatement(symbolTable, treeGetChild(statement, 1));
			return false;
		case LL_FOR:
			parseTreeSetLabel(statement, symbolTableGetNumFor(symbolTable));
			symbolTableIncrFor(symbolTable);
			
			Tree * loopIterator = treeGetChild(statement, 0);
			int variableType = checkVariableName(symbolTable, loopIterator, RIGHT_SIDE, 0);
			
			Tree * expression1 = treeGetChild(statement, 1);
			checkExpressionIsVariableType(symbolTable, expression1, loopIterator, variableType);

			Tree * expression2 = treeGetChild(statement, 2);
			checkExpressionIsVariableType(symbolTable, expression2, loopIterator, variableType);

			return checkStatement(symbolTable, treeGetChild(statement, 3));
		default:
			;
			return checkStatements(symbolTable, statement);
	}
}

static void checkConditionIsBoolean (SymbolTable * symbolTable, Tree * condition) {
	int expressionType = checkExpression(symbolTable, condition);
	if (expressionType != LL_BOOLEAN) {
		char * expressionTypeString = lexConstantToStringNoLL(expressionType);

		int lineDeclared = parseTreeGetLineNumberDeclared(condition);
		int lineIndexDeclared = parseTreeGetLineIndexDeclared(condition);
		char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
		fprintf(stderr, "[ERROR] CONDITION is not of type BOOLEAN at (%i,%i). [TRACE: %s]\n", 
			lineDeclared, lineIndexDeclared, trace);
		exit(1);
	}
	int numberOfRegisters = parseTreeGetLabel(condition);
	symbolTableUpdateTempRegs(symbolTable, numberOfRegisters);
} 

static void checkExpressionIsVariableType(SymbolTable * symbolTable, Tree * expression, Tree * variable, int variableType ) {
	int expressionType = checkExpression(symbolTable, expression);
	if (expressionType != variableType && expressionType != LL_NUM) {
		char * variableName 		= parseTreeGetAttribute(variable);
		char * variableTypeString 	= lexConstantToStringNoLL(variableType);
		char * expressionTypeString = lexConstantToStringNoLL(expressionType);

		int lineDeclared = parseTreeGetLineNumberDeclared(variable);
		int lineIndexDeclared = parseTreeGetLineIndexDeclared(variable);
		char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
		fprintf(stderr, "[ERROR] ASSIGNMENT of variable \"%s\" of type \"%s\" does not match left hand side type of \"%s\" at (%i,%i). [TRACE: %s]\n", 
			variableName, variableTypeString, expressionTypeString, lineDeclared, lineIndexDeclared, trace);
		exit(1);
	}
	int numberOfRegisters = parseTreeGetLabel(expression);
	symbolTableUpdateTempRegs(symbolTable, numberOfRegisters);
}


static bool checkAssignment(SymbolTable * symbolTable, Tree * assignmentStatement) {
	Tree * variable = treeGetChild(assignmentStatement, 0);
	Tree * expression = treeGetChild(assignmentStatement, 1);

	int variableType = checkVariableName(symbolTable, variable, LEFT_SIDE, 0);
	int numberOfRegisters = parseTreeGetLabel(variable);
	symbolTableUpdateTempRegs(symbolTable, numberOfRegisters);

	int expressionType = checkExpression(symbolTable, expression);
	numberOfRegisters = parseTreeGetLabel(expression);
	symbolTableUpdateTempRegs(symbolTable, numberOfRegisters);

	char * variableName 		= parseTreeGetAttribute(variable);
	if (expressionType != variableType && expressionType != LL_NUM) {
		char * variableTypeString 	= lexConstantToStringNoLL(variableType);
		char * expressionTypeString = lexConstantToStringNoLL(expressionType);

		int lineDeclared = parseTreeGetLineNumberDeclared(variable);
		int lineIndexDeclared = parseTreeGetLineIndexDeclared(variable);
		char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
		fprintf(stderr, "[ERROR] ASSIGNMENT of variable \"%s\" of type \"%s\" does not match left hand side type of \"%s\" at (%i,%i). [TRACE: %s]\n", 
			variableName, variableTypeString, expressionTypeString, lineDeclared, lineIndexDeclared, trace);
		exit(1);
	}

	SymbolTableScope * currentScope = symbolTablePeakScope(symbolTable);
	Tree * scopeLocation = currentScope->scopeLocation;
	bool isFunctionScope = parseTreeGetType(scopeLocation) == LL_FUNCTION;
	char * scopeName = currentScope->name;

	if (isFunctionScope && strcmp(variableName, scopeName) == 0) {
		parseTreeSetLabel(assignmentStatement, 1);
		return true;
	} else {
		parseTreeSetLabel(assignmentStatement, 0);
		return false;
	}
}

static void addVariables(SymbolTable * symbolTable, Tree * variableLocations) {
	int numberOfVariables = 0;
	if(!parseTreeIsNull(variableLocations)) {
		int listSize = treeGetSize(variableLocations) / 2;
		Iterator * iterator = linkedListIteratorInit(treeGetChildren(variableLocations));
		
		for(int i = 0; i < listSize; i++) {

			Tree * identifier_list 	= (Tree *) iteratorGetNext(iterator);
			Tree * typeTree 		= (Tree *) iteratorGetNext(iterator);
			int type = parseTreeGetType(typeTree);

			if( type == LL_ARRAY ) { // check array bounds
				Tree * lowerBoundLocation = treeGetChild(typeTree, 0);
				Tree * upperBoundLocation = treeGetChild(typeTree, 1);
				char * lowerBoundString = parseTreeGetAttribute(lowerBoundLocation);
				char * upperBoundString = parseTreeGetAttribute(upperBoundLocation);
				int upperBound = stringToInt(upperBoundString);
				int lowerBound = stringToInt(lowerBoundString);
								

				if (lowerBound > upperBound) {
					int lineDeclared = parseTreeGetLineNumberDeclared(typeTree);
					int lineIndexDeclared = parseTreeGetLineIndexDeclared(typeTree);
					char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
					fprintf(stderr, "[ERROR] ARRAY TYPE is declared with a lower bound (%i) that is greater than its upperbound (%i) at (%i,%i). [TRACE: %s]\n", 
						lowerBound, upperBound, lineDeclared, lineIndexDeclared, trace);
					exit(1);
				}

				parseTreeSetLabel(lowerBoundLocation, lowerBound);
				parseTreeSetLabel(upperBoundLocation, upperBound);
			}

			int identifier_size = treeGetSize(identifier_list);
			Iterator * identifier_iterator = linkedListIteratorInit(treeGetChildren(identifier_list));
			for(int j = 0; j < identifier_size; j++) {
				Tree * identifier 	= (Tree *) iteratorGetNext(identifier_iterator);
				char * variableName = parseTreeGetAttribute(identifier);
				SearchResult searchResult;
				
				bool conflict = symbolTableSearchScope(symbolTable, variableName, &searchResult);
				
				if(conflict) {
					Tree * variableValue = searchResult.attributes->variableValue;
					int previousLineDeclared = parseTreeGetLineNumberDeclared(variableValue);
					int previousLineIndexDeclared = parseTreeGetLineIndexDeclared(variableValue);
					int lineDeclared = parseTreeGetLineNumberDeclared(identifier);
					int lineIndexDeclared = parseTreeGetLineIndexDeclared(identifier);
					char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
					fprintf(stderr, "[ERROR] VARIABLE \"%s\" at (%i,%i) is already declared at (%i,%i). [TRACE: %s]\n", 
						variableName, lineDeclared, lineIndexDeclared, previousLineDeclared, previousLineIndexDeclared, trace);
					exit(1);
				}
				symbolTablePut(symbolTable, identifier, typeTree);
				numberOfVariables++;
			}
			iteratorDestroy(identifier_iterator);
		}
		iteratorDestroy(iterator);
	}
	parseTreeSetLabel(variableLocations, numberOfVariables);
}


static void addSubPrograms(SymbolTable * symbolTable, Tree * programLocations) {
	int numberOfSubPrograms = 0;
	if(!parseTreeIsNull(programLocations)) {
		int listSize = treeGetSize(programLocations);
		Iterator * iterator = linkedListIteratorInit(treeGetChildren(programLocations));
		while(iteratorHasNext(iterator)){
			Tree * sub_program = (Tree *) iteratorGetNext(iterator);
			char * program_name = parseTreeGetAttribute(sub_program);

			SearchResult searchResult;
			bool conflict = symbolTableSearchScope(symbolTable, program_name, &searchResult);
			if(conflict) {
				Tree * variableValue = searchResult.attributes->variableValue;
				int previousLineDeclared = parseTreeGetLineNumberDeclared(variableValue);
				int previousLineIndexDeclared = parseTreeGetLineIndexDeclared(variableValue);
				int type = parseTreeGetType(sub_program);
				char * programTypeString = lexConstantToString(type);
				programTypeString = stringTakeLast(programTypeString, 2); 
				int lineDeclared = parseTreeGetLineNumberDeclared(sub_program);
				int lineIndexDeclared = parseTreeGetLineIndexDeclared(sub_program);
				char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
				fprintf(stderr, "[ERROR] %s \"%s\" at (%i,%i) is already declared at (%i,%i). [TRACE: %s]\n", 
					programTypeString, program_name, lineDeclared, lineIndexDeclared,  previousLineDeclared, previousLineIndexDeclared, trace);
				exit(1);
			}
			symbolTableCreateScope(symbolTable, sub_program);
			numberOfSubPrograms++;
		}
		iteratorDestroy(iterator);
	}
	parseTreeSetLabel(programLocations, numberOfSubPrograms);
}


static int checkSubProgramName( SymbolTable * symbolTable, Tree * subProgram, int type ) {
	SymbolTableScope * currentScope = symbolTablePeakScope(symbolTable);
	Tree * scopeLocation = currentScope->scopeLocation;
	bool isFunctionScope = parseTreeGetType(scopeLocation) == LL_FUNCTION;

	char * program_name = parseTreeGetAttribute(subProgram);
	SearchResult searchResult;
	VariableAttr attr; // declared only below

	bool foundName = symbolTableSearchAll(symbolTable, program_name, &searchResult);
	
	if(!foundName) {
		char * scopeName = currentScope->name; 

		if( isFunctionScope && (strcmp(scopeName, program_name) == 0) ) {
			attr.variableValue = scopeLocation;
			attr.variableType  = scopeLocation;
			searchResult.attributes = &attr; // declared here
		} else {
			char * programTypeString = lexConstantToString(type);
			programTypeString = stringTakeLast(programTypeString, 2); 
			int lineDeclared = parseTreeGetLineNumberDeclared(subProgram);
			int lineIndexDeclared = parseTreeGetLineIndexDeclared(subProgram);
			char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
			fprintf(stderr, "[ERROR] %s \"%s\" was called at (%i,%i), but was never declared. [TRACE: %s]\n", 
				programTypeString, program_name, lineDeclared, lineIndexDeclared, trace);
			exit(1);
		}
	}


	int actualType = parseTreeGetType(searchResult.attributes->variableValue);
	
	if(type == LL_PROCEDURE && (actualType == READ_PROCEDURE_ID || actualType == WRITE_PROCEDURE_ID)) {
		if(treeIsLeaf(subProgram)) {
			int lineDeclared = parseTreeGetLineNumberDeclared(subProgram);
			int lineIndexDeclared = parseTreeGetLineIndexDeclared(subProgram);
			char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
			fprintf(stderr, "[ERROR] \"%s\" procedure at (%i,%i) was called with no arguments. [TRACE: %s]\n",
				program_name, lineDeclared, lineIndexDeclared, trace);
			exit(1); 
		}
		if(actualType == READ_PROCEDURE_ID) {
			
			Tree * expressionList = (Tree *) treeGetChild(subProgram, 0);
			Iterator * iterator = linkedListIteratorInit(treeGetChildren(expressionList));
			
			int i = 1;
			while(iteratorHasNext(iterator)) {
				Tree * item = iteratorGetNext(iterator);
				int itemType = parseTreeGetType(item);
				if(itemType != LL_ID || !treeIsLeaf(item)) {
					int lineDeclared = parseTreeGetLineNumberDeclared(item);
					int lineIndexDeclared = parseTreeGetLineIndexDeclared(item);
					char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
					fprintf(stderr, "[ERROR] Argument %i of the \"read\" procedure did not take in a variable name at (%i,%i). [TRACE: %s]\n",
						i, lineDeclared, lineIndexDeclared, trace);
					exit(1);
				}

				checkVariableName(symbolTable, item, RIGHT_SIDE, 0);

				SearchResult secondSearchResult;
				char * variableName = parseTreeGetAttribute(item);
				bool foundName = symbolTableSearchAll(symbolTable, variableName, &secondSearchResult);
				if(foundName) {
					Tree * variableTypeLocation = secondSearchResult.attributes->variableType;
					int variableType = parseTreeGetType(variableTypeLocation);
					if(variableType == LL_ARRAY) {
						Tree * variableValueLocation = secondSearchResult.attributes->variableValue;
						int previousLineDeclared = parseTreeGetLineNumberDeclared(variableValueLocation);
						int previousLineIndexDeclared = parseTreeGetLineIndexDeclared(variableValueLocation);
						int lineDeclared = parseTreeGetLineNumberDeclared(item);
						int lineIndexDeclared = parseTreeGetLineIndexDeclared(item);
						char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
						fprintf(stderr, "[ERROR] ARRAY \"%s\" defined at (%i,%i) was used as a argument (%i) to a \"read\" procedure at (%i,%i). [TRACE: %s]\n", 
							variableName,  previousLineDeclared, previousLineIndexDeclared, i, lineDeclared, lineIndexDeclared, trace);
						exit(1);
					}
				}
				i++;
			}

			iteratorDestroy(iterator);
			parseTreeSetType(subProgram, actualType);
			return LL_PROCEDURE; // no more checking for read procedure
		}

		parseTreeSetType(subProgram, actualType);

	} else if( actualType != type ) { // check if type is right
		Tree * variableValue = searchResult.attributes->variableValue;
		int previousLineDeclared = parseTreeGetLineNumberDeclared(variableValue);
		int previousLineIndexDeclared = parseTreeGetLineIndexDeclared(variableValue);
		char * programTypeString = lexConstantToString(type);
		programTypeString = stringTakeLast(programTypeString, 2);
		char * actualProgramTypeString;
		if(actualType != LL_ID) {
			actualProgramTypeString = lexConstantToString(actualType);
			actualProgramTypeString = stringTakeLast(actualProgramTypeString, 2);	
		} else{
			actualProgramTypeString = "VARIABLE";
		}
		int lineDeclared = parseTreeGetLineNumberDeclared(subProgram);
		int lineIndexDeclared = parseTreeGetLineIndexDeclared(subProgram);
		char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
		fprintf(stderr, "[ERROR] %s \"%s\" defined at (%i,%i) was called as a %s at (%i,%i). [TRACE: %s]\n", 
			actualProgramTypeString, program_name, previousLineDeclared, previousLineIndexDeclared, programTypeString, lineDeclared, lineIndexDeclared, trace);
		exit(1);
	}
	

	if(! treeIsLeaf(subProgram) ) { // check arguments
		Tree * variableValue = searchResult.attributes->variableValue;
		Tree * parameters = treeGetChild(variableValue, 0);
		Tree * expressionList = treeGetChild(subProgram, 0);
		if( actualType != WRITE_PROCEDURE_ID ) { // check number of arguments
			int numberOfParameters = parseTreeGetLabel( parameters );
			int numberOfArguments = treeGetSize( expressionList ) ;
			if ( numberOfArguments != numberOfParameters ) {
				int previousLineDeclared = parseTreeGetLineNumberDeclared(variableValue);
				int previousLineIndexDeclared = parseTreeGetLineIndexDeclared(variableValue);
				int lineDeclared = parseTreeGetLineNumberDeclared(subProgram);
				int lineIndexDeclared = parseTreeGetLineIndexDeclared(subProgram);
				char * actualProgramTypeString = lexConstantToString(actualType);
				actualProgramTypeString = stringTakeLast(actualProgramTypeString, 2);
				char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
				fprintf(stderr, "[ERROR] %s \"%s\" defined at (%i,%i) was called with %i arguments when it should have %i parameters at (%i,%i). [TRACE: %s]\n", 
				actualProgramTypeString, program_name, previousLineDeclared, previousLineIndexDeclared, numberOfArguments, numberOfParameters, lineDeclared, lineIndexDeclared, trace);
				exit(1);
			}
			Iterator * parameterTypeIterator = linkedListIteratorInit(treeGetChildren(parameters));
			Tree * identifierList = iteratorGetNext(parameterTypeIterator);
			Tree * identifierType = iteratorGetNext(parameterTypeIterator);
			Iterator * parameterValueIterator = linkedListIteratorInit(treeGetChildren(identifierList));

			Iterator * argumentIterator = linkedListIteratorInit(treeGetChildren(expressionList));
			
			int currentParameterType = parseTreeGetType(identifierType);

			int currentArgumentIndex = 1;
			while (iteratorHasNext(argumentIterator)) {
				if( ! iteratorHasNext(parameterValueIterator) ) {
					identifierList = iteratorGetNext(parameterTypeIterator);
					identifierType = iteratorGetNext(parameterTypeIterator);
					iteratorDestroy(parameterValueIterator);
					parameterValueIterator = linkedListIteratorInit(treeGetChildren(identifierList));
					currentParameterType = parseTreeGetType(identifierType);
				}

				iteratorGetNext(parameterValueIterator);
				Tree * argument = iteratorGetNext(argumentIterator);
				if( currentParameterType == LL_ARRAY ) { // expects an array as input
					int argumentType = parseTreeGetType(argument);
					if( argumentType != LL_ID ) { // checks is expression is called
						int previousLineDeclared = parseTreeGetLineNumberDeclared(variableValue);
						int previousLineIndexDeclared = parseTreeGetLineIndexDeclared(variableValue);
						int lineDeclared = parseTreeGetLineNumberDeclared(argument);
						int lineIndexDeclared = parseTreeGetLineIndexDeclared(argument);
						char * actualProgramTypeString = lexConstantToString(actualType);
						actualProgramTypeString = stringTakeLast(actualProgramTypeString, 2);
						char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
						fprintf(stderr, "[ERROR] %s \"%s\" defined at (%i,%i) was called having argument %i not being of type ARRAY at (%i,%i). [TRACE: %s]\n", 
						actualProgramTypeString, program_name, previousLineDeclared, previousLineIndexDeclared, currentArgumentIndex, lineDeclared, lineIndexDeclared, trace);
						exit(1);
					}
					checkVariableName(symbolTable, argument, RIGHT_SIDE, identifierType);
					

					parseTreeSetLabel(argument, ARRAY_TYPE_ARGUMENT_LABEL);			// TODO: MIGHT HAVE TO CHANGE THIS
				} else {
					int argumentType = checkExpression(symbolTable, argument);
					if (argumentType != currentParameterType && argumentType != LL_NUM) {
						char * expectedTypeString = lexConstantToStringNoLL(currentParameterType);
						char * actualTypeString = lexConstantToStringNoLL(argumentType);
						int previousLineDeclared = parseTreeGetLineNumberDeclared(variableValue);
						int previousLineIndexDeclared = parseTreeGetLineIndexDeclared(variableValue);
						int lineDeclared = parseTreeGetLineNumberDeclared(argument);
						int lineIndexDeclared = parseTreeGetLineIndexDeclared(argument);
						char * actualProgramTypeString = lexConstantToString(actualType);
						actualProgramTypeString = stringTakeLast(actualProgramTypeString, 2);
						char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
						fprintf(stderr, "[ERROR] %s \"%s\" defined at (%i,%i) was called having argument %i being of type %s when it is expected to have type %s at (%i,%i). [TRACE: %s]\n", 
						actualProgramTypeString, program_name, previousLineDeclared, previousLineIndexDeclared, currentArgumentIndex, actualTypeString, expectedTypeString, lineDeclared, lineIndexDeclared, trace);
						exit(1);
					}
				}

				int label = parseTreeGetLabel(argument);
				if (type == LL_FUNCTION) {
					int functionLabel = parseTreeGetLabel(subProgram);
					if(label > functionLabel)
						parseTreeSetLabel(subProgram, label);  // TODO: MIGHT HAVE TO CHANGE THIS
				} else {
					symbolTableUpdateTempRegs(symbolTable, label);
				}
				currentArgumentIndex++;
			}
			iteratorDestroy(parameterValueIterator); 
			iteratorDestroy(parameterTypeIterator);
			iteratorDestroy(argumentIterator);

		} else {

			Iterator * argumentIterator = linkedListIteratorInit(treeGetChildren(expressionList));

			while (iteratorHasNext(argumentIterator)) {
				Tree * argument = iteratorGetNext(argumentIterator);
				checkExpression(symbolTable, argument);
				int label = parseTreeGetLabel(argument);
				symbolTableUpdateTempRegs(symbolTable, label);
			}
			iteratorDestroy(argumentIterator);

		}
	} else {  // no arguments are present
		Tree * variableValue = searchResult.attributes->variableValue; 
		Tree * parameters = treeGetChild(variableValue, 0);
		int numberOfParameters = parseTreeGetLabel( parameters );
		if ( numberOfParameters != 0 ) {
			int previousLineDeclared = parseTreeGetLineNumberDeclared(variableValue);
			int previousLineIndexDeclared = parseTreeGetLineIndexDeclared(variableValue);
			int lineDeclared = parseTreeGetLineNumberDeclared(subProgram);
			int lineIndexDeclared = parseTreeGetLineIndexDeclared(subProgram);
			char * actualProgramTypeString = lexConstantToString(actualType);
			actualProgramTypeString = stringTakeLast(actualProgramTypeString, 2);
			char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
			fprintf(stderr, "[ERROR] %s \"%s\" defined at (%i,%i) was called with no arguments when it should have %i parameters at (%i,%i). [TRACE: %s]\n", 
			actualProgramTypeString, program_name, previousLineDeclared, previousLineIndexDeclared, numberOfParameters, lineDeclared, lineIndexDeclared, trace);
			exit(1);
		} 
		if(type == LL_FUNCTION)
			parseTreeSetLabel(subProgram, FUNCTION_WITH_NO_PARAMETERS);			// TODO: MIGHT HAVE TO CHANGE THIS
	}


	if (type == LL_FUNCTION) {
		Tree * variableValue = searchResult.attributes->variableValue; 
		Tree * returnTypeLocation = treeGetChild(variableValue, 1);
		int returnType = parseTreeGetType(returnTypeLocation);
		return returnType;
	} else {
		return LL_PROCEDURE;
	}

}


static int checkExpression(SymbolTable * symbolTable, Tree * expression) {
	int expressionType = parseTreeGetType(expression);

	if (expressionType == LL_ID) {
		SymbolTableScope * currentScope = symbolTablePeakScope(symbolTable);
		Tree * scopeLocation = currentScope->scopeLocation;
		bool isFunctionScope = parseTreeGetType(scopeLocation) == LL_FUNCTION;
		SearchResult searchResult;
		char * variableName = parseTreeGetAttribute(expression);
		bool foundResult = symbolTableSearchAll(symbolTable, variableName, &searchResult);

		VariableAttr attr;

		if (!foundResult && isFunctionScope && strcmp(currentScope->name, variableName) == 0) {
			attr.variableValue = scopeLocation;
			attr.variableType = scopeLocation;
			searchResult.attributes = &attr;
			foundResult = true;
		}


		if(!foundResult) { // dummy code set up for error message
			if ( treeIsLeaf (expression) ) {
				checkVariableName(symbolTable, expression, RIGHT_SIDE, 0);
			} else{
				Tree * child = treeGetChild(expression, 0);
				int childType = parseTreeGetType(child);
				if (childType < LL_PROGRAM)
					checkSubProgramName(symbolTable, expression, LL_FUNCTION);
				else
					checkVariableName(symbolTable, expression, RIGHT_SIDE, 0);
			}
		} else {
			Tree * actualTypeLocation = searchResult.attributes->variableValue;
			int actualType = parseTreeGetType(actualTypeLocation);

			if(actualType != LL_ID) {
				int returnType = checkSubProgramName(symbolTable, expression, LL_FUNCTION);
				return returnType;
			} else {
				int variableType = checkVariableName(symbolTable, expression, RIGHT_SIDE, 0);
				return variableType;
			}
		}
	} else if (expressionType == LL_NUM) {
		parseTreeSetLabel(expression, SINGLE_NODE_EXPRESSION);
		return LL_NUM;
	} else {

		int childrenSize = treeGetSize(expression);
		char * expressionOperatorString = parseTreeGetAttribute(expression);
		if(childrenSize == 1) {
			Tree * onlyChild = treeGetChild(expression, 0);
			int valueType = checkExpression(symbolTable, onlyChild);

			// type checking
			if (strcmp(expressionOperatorString, "not") == 0 && valueType != LL_BOOLEAN) {
				char * typeString = lexConstantToStringNoLL(valueType);

				int lineDeclared = parseTreeGetLineNumberDeclared(expression);
				int lineIndexDeclared = parseTreeGetLineIndexDeclared(expression);
				char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
				fprintf(stderr, "[ERROR] OPERATOR \"NOT\" is not defined with operand of type \"%s\" at (%i,%i). [TRACE: %s]\n", 
					 typeString, lineDeclared, lineIndexDeclared, trace);
				exit(1);
			}

			// labeling
			int childLabel = parseTreeGetLabel(onlyChild);
			parseTreeSetLabel(expression, childLabel);
			return valueType;
		} else {
			Tree * leftChild = treeGetChild(expression, 0);
			Tree * rightChild = treeGetChild(expression, 1);
			
			int leftChildType = checkExpression(symbolTable, leftChild);
			int rightChildType = checkExpression(symbolTable, rightChild);

			int totalType;
			// type checking
			if (strcmp(expressionOperatorString, "and") == 0 || strcmp(expressionOperatorString, "or") == 0) {
				if(leftChildType != LL_BOOLEAN || rightChildType != LL_BOOLEAN) {
					char * leftTypeString = lexConstantToStringNoLL(leftChildType); 
					char * rightTypeString = lexConstantToStringNoLL(rightChildType);

					int lineDeclared = parseTreeGetLineNumberDeclared(expression);
					int lineIndexDeclared = parseTreeGetLineIndexDeclared(expression);
					char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
					fprintf(stderr, "[ERROR] OPERATOR \"%s\" is not defined with having a left side operand of type \"%s\" and a right side operand of type \"%s\" at (%i,%i). [TRACE: %s]\n", 
						 expressionOperatorString, leftTypeString, rightTypeString, lineDeclared, lineIndexDeclared, trace);
					exit(1);
				}
				totalType = LL_BOOLEAN;
			} else {
				if (leftChildType == LL_BOOLEAN || rightChildType == LL_BOOLEAN) {
					char * leftTypeString = lexConstantToStringNoLL(leftChildType); 
					char * rightTypeString = lexConstantToStringNoLL(rightChildType);

					int lineDeclared = parseTreeGetLineNumberDeclared(expression);
					int lineIndexDeclared = parseTreeGetLineIndexDeclared(expression);
					char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
					fprintf(stderr, "[ERROR] OPERATOR \"%s\" is not defined with having a left side operand of type \"%s\" and a right side operand of type \"%s\" at (%i,%i). [TRACE: %s]\n", 
						 expressionOperatorString, leftTypeString, rightTypeString, lineDeclared, lineIndexDeclared, trace);
					exit(1);
				}
				if ( leftChildType == LL_NUM ) {
					totalType = rightChildType;
				} else if (rightChildType == LL_NUM) {
					totalType = leftChildType;
				} else {
					if (leftChildType != rightChildType) {
						char * leftTypeString = lexConstantToStringNoLL(leftChildType); 
						char * rightTypeString = lexConstantToStringNoLL(rightChildType);

						int lineDeclared = parseTreeGetLineNumberDeclared(expression);
						int lineIndexDeclared = parseTreeGetLineIndexDeclared(expression);
						char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
						fprintf(stderr, "[ERROR] OPERATOR \"%s\" is not defined with having a left side operand of type \"%s\" and a right side operand of type \"%s\" at (%i,%i). [TRACE: %s]\n", 
							 expressionOperatorString, leftTypeString, rightTypeString, lineDeclared, lineIndexDeclared, trace);
						exit(1);
					}
					totalType = leftChildType;
				}
				int expressionType = parseTreeGetType(expression);
				if (expressionType == LL_RELOP)
					totalType = LL_BOOLEAN;
			}

			// labeling
			int leftChildLabel = parseTreeGetLabel(leftChild);
			int rightChildLabel = parseTreeGetLabel(rightChild);
			if (leftChildLabel == 0) {
				parseTreeSetLabel(leftChild, 1);
				leftChildLabel = 1;	
			}
			if (leftChildLabel != rightChildLabel) {
				if (leftChildLabel > rightChildLabel) {
					parseTreeSetLabel(expression, leftChildLabel);
				} else {
					parseTreeSetLabel(expression, rightChildLabel);
				}
			} else {
				parseTreeSetLabel(expression, leftChildLabel + 1);
			}
			return totalType;
		}
	}
	return -1;
}


// expected variable name here 
static int checkVariableName(SymbolTable * symbolTable, Tree * variable, bool sideCalled, Tree * arrayType) {
	SymbolTableScope * currentScope = symbolTablePeakScope(symbolTable);
	Tree * scopeLocation = currentScope->scopeLocation;
	bool isFunctionScope = parseTreeGetType(scopeLocation) == LL_FUNCTION;


	char * variable_name = parseTreeGetAttribute(variable);
	SearchResult searchResult;

	bool foundName = symbolTableSearchAll(symbolTable, variable_name, &searchResult);

	if(!foundName) {
		if( sideCalled == LEFT_SIDE ) {

			if (strcmp(currentScope->name, variable_name) == 0) {
				if ( !isFunctionScope ) {
					int previousLineDeclared = parseTreeGetLineNumberDeclared(scopeLocation);
					int previousLineIndexDeclared = parseTreeGetLineIndexDeclared(scopeLocation);
					int lineDeclared = parseTreeGetLineNumberDeclared(variable);
					int lineIndexDeclared = parseTreeGetLineIndexDeclared(variable);
					char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
					fprintf(stderr, "[ERROR] PROCEDURE \"%s\" defined at (%i,%i) it attempting to RETURN a value at (%i,%i). [TRACE: %s]\n", 
						variable_name, previousLineDeclared, previousLineIndexDeclared, lineDeclared, lineIndexDeclared, trace);
					exit(1);
				}

				if(!treeIsLeaf(variable)) {
					int previousLineDeclared = parseTreeGetLineNumberDeclared(scopeLocation);
					int previousLineIndexDeclared = parseTreeGetLineIndexDeclared(scopeLocation);
					int lineDeclared = parseTreeGetLineNumberDeclared(variable);
					int lineIndexDeclared = parseTreeGetLineIndexDeclared(variable);
					char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
					fprintf(stderr, "[ERROR] FUNCTION \"%s\" defined at (%i,%i) has its RETURN VARIABLE indexed as a ARRAY at (%i,%i). [TRACE: %s]\n", 
						variable_name, previousLineDeclared, previousLineIndexDeclared, lineDeclared, lineIndexDeclared, trace);
					exit(1);
				}
				Tree * returnTypeLocation = treeGetChild(scopeLocation, 1);
				int returnType = parseTreeGetType(returnTypeLocation);

				return returnType; // it is fine
			}
		}

		int lineDeclared = parseTreeGetLineNumberDeclared(variable);
		int lineIndexDeclared = parseTreeGetLineIndexDeclared(variable);
		char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
		fprintf(stderr, "[ERROR] VARIABLE \"%s\" was referenced at (%i,%i), but was never declared. [TRACE: %s]\n", 
			 variable_name, lineDeclared, lineIndexDeclared, trace);
		exit(1);
	}
	
	int actualType = parseTreeGetType(searchResult.attributes->variableValue);

	if( actualType != LL_ID ) { // checks if function or procedure
		Tree * variableValue = searchResult.attributes->variableValue;
		int previousLineDeclared = parseTreeGetLineNumberDeclared(variableValue);
		int previousLineIndexDeclared = parseTreeGetLineIndexDeclared(variableValue);
		char * actualProgramTypeString = lexConstantToString(actualType);
		actualProgramTypeString = stringTakeLast(actualProgramTypeString, 2);
		int lineDeclared = parseTreeGetLineNumberDeclared(variable);
		int lineIndexDeclared = parseTreeGetLineIndexDeclared(variable);
		char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
		fprintf(stderr, "[ERROR] %s \"%s\" defined at (%i,%i) was referenced as a VARIABLE at (%i,%i). [TRACE: %s]\n", 
			actualProgramTypeString, variable_name, previousLineDeclared, previousLineIndexDeclared, lineDeclared, lineIndexDeclared, trace);
		exit(1);
	}

	Tree * actualTypeLocation = searchResult.attributes->variableType;
	int type = parseTreeGetType(actualTypeLocation);
	
	if (type != LL_ARRAY && arrayType) { // type is not a array but it is being checked as a array
		Tree * variableValue = searchResult.attributes->variableValue;
		int previousLineDeclared = parseTreeGetLineNumberDeclared(arrayType);
		int previousLineIndexDeclared = parseTreeGetLineIndexDeclared(arrayType);
		int lineDeclared = parseTreeGetLineNumberDeclared(variable);
		int lineIndexDeclared = parseTreeGetLineIndexDeclared(variable);
		char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
		fprintf(stderr, "[ERROR] NON-ARRAY VARIABLE \"%s\" declared at (%i,%i) is being called where an array should be present at (%i,%i). [TRACE: %s]\n", 
			variable_name, previousLineDeclared, previousLineIndexDeclared, lineDeclared, lineIndexDeclared, trace);
		exit(1);
	}

	if(type != LL_ARRAY && ! treeIsLeaf(variable) ) { // indexing an array that is not a array
		Tree * variableValue = searchResult.attributes->variableValue;
		int previousLineDeclared = parseTreeGetLineNumberDeclared(variableValue);
		int previousLineIndexDeclared = parseTreeGetLineIndexDeclared(variableValue);
		int lineDeclared = parseTreeGetLineNumberDeclared(variable);
		int lineIndexDeclared = parseTreeGetLineIndexDeclared(variable);
		char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
		fprintf(stderr, "[ERROR] NON-ARRAY VARIABLE \"%s\" defined at (%i,%i) was referenced as a ARRAY at (%i,%i). [TRACE: %s]\n", 
			variable_name, previousLineDeclared, previousLineIndexDeclared, lineDeclared, lineIndexDeclared, trace);
		exit(1);
	} else if ( type == LL_ARRAY && treeIsLeaf(variable) ) { // type is an array, but the whole array is called
		if( ! arrayType ) {     // not search for whole array 
			Tree * variableValue = searchResult.attributes->variableValue;
			int previousLineDeclared = parseTreeGetLineNumberDeclared(variableValue);
			int previousLineIndexDeclared = parseTreeGetLineIndexDeclared(variableValue);
			int lineDeclared = parseTreeGetLineNumberDeclared(variable);
			int lineIndexDeclared = parseTreeGetLineIndexDeclared(variable);
			char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
			fprintf(stderr, "[ERROR] ARRAY VARIABLE \"%s\" defined at (%i,%i) was referenced as a WHOLE ARRAY when it shouldn't have at (%i,%i). [TRACE: %s]\n", 
				variable_name, previousLineDeclared, previousLineIndexDeclared, lineDeclared, lineIndexDeclared, trace);
			exit(1);
		} else {
			Tree * actualTypeTypeLocation = searchResult.attributes->variableType;
			Tree * arrayTypeType = treeGetChild(actualTypeTypeLocation, 2);
			int actualTypeType = parseTreeGetType(arrayTypeType);
			Tree * expectedArrayType = treeGetChild(arrayType, 2);
			int expectedArrayTypeValue = parseTreeGetType(expectedArrayType);
			if (expectedArrayTypeValue != actualTypeType) {
				char * expectedTypeString = lexConstantToStringNoLL(expectedArrayTypeValue);
				char * actualTypeString   = lexConstantToStringNoLL(actualTypeType);
				int previousLineDeclared = parseTreeGetLineNumberDeclared(arrayType);
				int previousLineIndexDeclared = parseTreeGetLineIndexDeclared(arrayType);
				int lineDeclared = parseTreeGetLineNumberDeclared(variable);
				int lineIndexDeclared = parseTreeGetLineIndexDeclared(variable);
				char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
				fprintf(stderr, "[ERROR] At (%i,%i), ARRAY \"%s\" of type \"%s\" does not match the expected ARRAY type \"%s\" at (%i,%i). [TRACE: %s]\n", 
					lineDeclared, lineIndexDeclared, variable_name, actualTypeString, expectedTypeString, previousLineDeclared, previousLineIndexDeclared, trace);
				exit(1);
			}

			Tree * actualVariableType = searchResult.attributes->variableType;
			Tree * actualLowerBoundLocation = treeGetChild(actualVariableType, 0);
			Tree * actualUpperBoundLocation = treeGetChild(actualVariableType, 1);
			Tree * lowerBoundLocation = treeGetChild(arrayType, 0);
			Tree * upperBoundLocation = treeGetChild(arrayType, 1);

			int actualUpperBound = parseTreeGetLabel(actualUpperBoundLocation);
			int actualLowerBound = parseTreeGetLabel(actualLowerBoundLocation);
			int upperBound = parseTreeGetLabel(upperBoundLocation);
			int lowerBound = parseTreeGetLabel(lowerBoundLocation);
			if(actualUpperBound != upperBound || actualLowerBound != lowerBound ) {
				int previousLineDeclared = parseTreeGetLineNumberDeclared(arrayType);
				int previousLineIndexDeclared = parseTreeGetLineIndexDeclared(arrayType);
				int lineDeclared = parseTreeGetLineNumberDeclared(variable);
				int lineIndexDeclared = parseTreeGetLineIndexDeclared(variable);
				char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
				fprintf(stderr, "[ERROR] At (%i,%i), ARRAY BOUNDS of \"%s\" (%i to %i) does not match the expected ARRAY BOUNDS (%i to %i) at (%i,%i). [TRACE: %s]\n", 
					lineDeclared, lineIndexDeclared, variable_name, actualLowerBound, actualUpperBound, lowerBound, upperBound, previousLineDeclared, previousLineIndexDeclared, trace);
				exit(1);
			}
			parseTreeSetLabel(variable, SINGLE_NODE_EXPRESSION);
			return LL_ARRAY;
		}

	} else if (type == LL_ARRAY && ! treeIsLeaf(variable)) {  // type is an array and is being indexed
		Tree * index = treeGetChild(variable, 0);
		int indexType = checkExpression(symbolTable, index);
		if (indexType != LL_INTEGER && indexType != LL_NUM) {
			Tree * variableValue = searchResult.attributes->variableValue;
			char * indexTypeString = lexConstantToStringNoLL(indexType);
			int previousLineDeclared = parseTreeGetLineNumberDeclared(variableValue);
			int previousLineIndexDeclared = parseTreeGetLineIndexDeclared(variableValue);
			int lineDeclared = parseTreeGetLineNumberDeclared(index);
			int lineIndexDeclared = parseTreeGetLineIndexDeclared(index);
			char * trace = symbolTableScopeTraceString(symbolTable, &tracePrintProperties);
			fprintf(stderr, "[ERROR] VARIABLE ARRAY \"%s\" declared at (%i,%i) was indexed with a NON-INTEGER value (%s) at (%i,%i). [TRACE: %s]\n", 
				 variable_name, previousLineDeclared, previousLineIndexDeclared, indexTypeString, lineDeclared, lineIndexDeclared, trace);
			exit(1);
		}
		int indexLabel = parseTreeGetLabel(index);
		parseTreeSetLabel(variable, indexLabel);

		Tree * actualTypeTypeLocation = searchResult.attributes->variableType;
		Tree * arrayTypeType = treeGetChild(actualTypeTypeLocation, 2);
		int actualTypeType = parseTreeGetType(arrayTypeType);
		return actualTypeType;

	}
	parseTreeSetLabel(variable, SINGLE_NODE_EXPRESSION);
	int variableType = parseTreeGetType(actualTypeLocation);
	return variableType;
}

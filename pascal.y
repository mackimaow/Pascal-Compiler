%{
#include <stdio.h>
#include "lexconstants.h"
#include "parsetree.h"
#include "y.tab.h"


extern int yylex();
extern int yyerror(char *);

static Tree * outputTree = 0;

Tree * getOutputTree() {
	return outputTree;
}


%}

%union {
  TreeValue * treeVal;
  Tree * tree;
}

%token <treeVal> L_PROGRAM
%token <treeVal> L_FUNCTION
%token <treeVal> L_PROCEDURE
%token <treeVal> L_IF
%token <treeVal> L_THEN
%token <treeVal> L_ELSE
%token <treeVal> L_WHILE
%token <treeVal> L_FOR
%token <treeVal> L_TO
%token <treeVal> L_DO
%token <treeVal> L_BEGIN
%token <treeVal> L_END
%token <treeVal> L_INTEGER
%token <treeVal> L_REAL
%token <treeVal> L_VAR
%token <treeVal> L_ARRAY
%token <treeVal> L_OF
%token <treeVal> L_NOT
%token <treeVal> L_ADD
%token <treeVal> L_SUB
%token <treeVal> L_OR
%token <treeVal> L_MULOP
%token <treeVal> L_RELOP
%token <treeVal> L_ASSIGNOP
%token <treeVal> L_ID
%token <treeVal> L_NUM
%token <treeVal> L_LP
%token <treeVal> L_RP
%token <treeVal> L_LB
%token <treeVal> L_RB
%token <treeVal> L_DD
%token <treeVal> L_D
%token <treeVal> L_COM
%token <treeVal> L_SC
%token <treeVal> L_C










%type <tree> identifier_list
%type <tree> declarations
%type <tree> type
%type <tree> standard_type
%type <tree> subprogram_declarations
%type <tree> subprogram_declaration
%type <tree> subprogram_head
%type <tree> arguments
%type <tree> parameter_list
%type <tree> compound_statement
%type <tree> optional_statements
%type <tree> statement_list
%type <tree> statement
%type <tree> matched_stmt
%type <tree> unmatched_stmt
%type <tree> variable
%type <tree> procedure_statement
%type <tree> expression_list
%type <tree> expression
%type <tree> simple_expression
%type <tree> term
%type <tree> factor




%start program

%%



program: L_PROGRAM    L_ID   L_LP identifier_list L_RP     L_SC   declarations  subprogram_declarations  compound_statement  L_D { outputTree = parseTreeInitCombine( $1 , $2, 4, TREE_NODE, $4, TREE_NODE, $7, TREE_NODE, $8, TREE_NODE, $9); } 
	;


identifier_list:  L_ID				{ $$ = parseTreeInitNew("IDENTIFIER_LIST", $1, 1, TREE_LEAF, $1 ); }
	| identifier_list L_COM L_ID    { $$ = parseTreeSafelyAddLeaf(0, 0, $1, $3); }
	;


declarations:  { $$ = parseTreeGetNullNode("<NO DECLARATIONS>"); }
    | declarations	L_VAR  identifier_list   L_C type	L_SC	{ $$ = parseTreeSafelyAddNodes("DECLARATIONS", $2, $1, 2, $3, $5); }
    ;


type: standard_type														{ $$ = $1; }
	| L_ARRAY  L_LB  L_NUM  L_DD  L_NUM   L_RB  L_OF standard_type 		{ $$ = parseTreeInit($1, 3, TREE_LEAF, $3, TREE_LEAF, $5, TREE_NODE, $8 ); }
	;


standard_type: L_INTEGER		{ $$ = parseTreeInit($1, 0); }
	| L_REAL		{ $$ = parseTreeInit($1, 0); }
	;


subprogram_declarations: { $$ = parseTreeGetNullNode("<NO SUBPROGRAM DECLARATIONS>"); }
	| subprogram_declarations subprogram_declaration L_SC	{ $$ = parseTreeSafelyAddNodes("SUBPROGRAM_DECLARATIONS", $3, $1, 1, $2); }
	;


subprogram_declaration: subprogram_head  declarations  subprogram_declarations  compound_statement  { $$ = parseTreeSafelyAddNodes(0, 0, $1, 3, $2, $3, $4); }
	;


subprogram_head: L_FUNCTION  L_ID  arguments  L_C  standard_type  L_SC 	{ $$ = parseTreeInitCombine( $1 , $2, 2, TREE_NODE, $3, TREE_NODE, $5); }
	| L_PROCEDURE  L_ID  arguments L_SC									{ $$ = parseTreeInitCombine( $1 , $2, 1, TREE_NODE, $3); }
	;


arguments: 							{ $$ = parseTreeGetNullNode("<NO ARGUMENTS>"); } 
	| L_LP  parameter_list  L_RP  	{ $$ = $2; }
	;


parameter_list: identifier_list  L_C  type  { $$ = parseTreeInitNew("PARAMETER_LIST", $2, 2, TREE_NODE, $1, TREE_NODE, $3 ); }
	| parameter_list  L_SC  identifier_list  L_C  type  {  $$ = parseTreeSafelyAddNodes(0, 0, $1, 2, $3, $5); }
	;


compound_statement: L_BEGIN  optional_statements  L_END  { $$ = $2; }
	;


optional_statements:  { $$ = parseTreeGetNullNode("<NO STATEMENTS>"); } 
	| statement_list  { $$ = $1; }
	;


statement_list:  statement  			{ $$ = parseTreeInitNew("STATEMENT_LIST", 0, 1, TREE_NODE, $1 ); }
	| statement_list  L_SC  statement 	{ $$ = parseTreeSafelyAddNode(0, 0, $1, $3); }
	;


statement:  matched_stmt		{ $$ = $1; }
	| unmatched_stmt			{ $$ = $1; }
	;


matched_stmt:  L_IF expression L_THEN matched_stmt L_ELSE matched_stmt				{ $$ = parseTreeInit($1, 3, TREE_NODE, $2, TREE_NODE, $4, TREE_NODE, $6); }
	|  	variable  L_ASSIGNOP  expression  											{ $$ = parseTreeInit($2, 2, TREE_NODE, $1, TREE_NODE, $3); }
	|	procedure_statement															{ $$ = $1; }
	|	compound_statement															{ $$ = $1; }
	|	L_WHILE  expression  L_DO  statement 										{ $$ = parseTreeInit($1, 2, TREE_NODE, $2, TREE_NODE, $4); }
	|	L_FOR  L_ID  L_ASSIGNOP  expression  L_TO  expression  L_DO  statement 		{ $$ = parseTreeInit($1, 4, TREE_LEAF, $2, TREE_NODE, $4, TREE_NODE, $6, TREE_NODE, $8); }
	;

unmatched_stmt: L_IF expression L_THEN  statement							{ $$ = parseTreeInit($1, 2, TREE_NODE, $2, TREE_NODE, $4); }
	| L_IF expression L_THEN matched_stmt L_ELSE unmatched_stmt				{ $$ = parseTreeInit($1, 3, TREE_NODE, $2, TREE_NODE, $4, TREE_NODE, $6); }
	;

variable:   L_ID						{  $$ = parseTreeInit($1, 0);  }
	|  L_ID  L_LB  expression  L_RB  	{  $$ = parseTreeInit($1, 1, TREE_NODE, $3);  }
	;


procedure_statement:  L_ID					{ $$ = parseTreeInit($1, 0); }
	|	L_ID  L_LP  expression_list  L_RP  	{ $$ = parseTreeInit($1, 1, TREE_NODE, $3); }
	;


expression_list:  expression 				{ $$ = parseTreeInitNew("EXPRESSION_LIST", 0, 1, TREE_NODE, $1 ); }
	|	expression_list  L_COM  expression 	{ $$ = parseTreeSafelyAddNode(0, 0, $1, $3); }
	;

expression:  simple_expression							{ $$ = $1; }
	| simple_expression  L_RELOP  simple_expression		{ $$ = parseTreeInit($2, 2, TREE_NODE, $1, TREE_NODE, $3); }
	;
	
simple_expression: term 			{ $$ = $1; }
	| L_SUB term 					{ $$ = parseTreeInit($1, 1, TREE_NODE, $2); }
	| simple_expression L_ADD term 	{ $$ = parseTreeInit($2, 2, TREE_NODE, $1, TREE_NODE, $3); }
	| simple_expression L_SUB term 	{ $$ = parseTreeInit($2, 2, TREE_NODE, $1, TREE_NODE, $3); }
	| simple_expression L_OR  term 	{ $$ = parseTreeInit($2, 2, TREE_NODE, $1, TREE_NODE, $3); }
	;

term: factor			{ $$ = $1; }
	| term L_MULOP factor	{ $$ = parseTreeInit($2, 2, TREE_NODE, $1, TREE_NODE, $3); }
	;

factor: variable 						{ $$ = $1; }
	| L_ID L_LP expression_list L_RP 	{ $$ = parseTreeInit($1, 1, TREE_NODE, $3); }
	| L_NUM 							{ $$ = parseTreeInit($1, 0); }
	| L_LP expression L_RP 				{ $$ = $2; }
	| L_NOT factor 						{ $$ = parseTreeInit($1, 1, TREE_NODE, $2); }
	;


%%


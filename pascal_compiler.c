#include <stdio.h>
#include <unistd.h>
#include "tree.h"
#include "parsetree.h"
#include "symboltable.h"
#include "semantic_analyzer.h"
#include "code_generator.h"
#include "y.tab.h"

extern FILE * yyin;
extern int yyparse();
extern Tree * getOutputTree();

int main(int argc, char *argv[]){
	if(argc > 2) {
		printf("Too many arguments\n");
		exit(1);	
	} else if (argc == 2){
		if( access( argv[1], F_OK ) == -1 ) { // file exists
			printf("File \"%s\" does not exist\n", argv[1]);
			exit(1);
		}
		printf("\n\nSTART PARSING\n\n");
		yyin = fopen( argv[1] , "r" );
		yyparse();
		fclose( yyin );
	} else {
		printf("\n\nSTART PARSING\n\n");
		yyparse();	
	}
  	printf("\n\nDONE PARSING\n\n");
  	Tree * tree = getOutputTree();

  	printf("\n\nSYNTAX TREE:\n\n");
  	treePrint(tree);

  	printf("\n\nSTART ANALYZING SEMANTICS\n\n");
  	SymbolTable * symbolTable = analyzeSemantics(tree);
  	printf("\n\nDONE ANALYZING\n\n");

  	printf("\n\nSTART CODE GENERATION\n\n");
  	char * code = generateCode(tree, symbolTable);
  	printf("\n\nEND CODE GENERATION\n\n");
  	
  	printf("\n\nCOMPILATION SUCCESFUL\n");

  	// end program
  	return 0;
}

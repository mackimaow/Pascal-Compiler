#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include "tree.h"
#include "parsetree.h"
#include "symboltable.h"
#include "semantic_analyzer.h"
#include "code_generator.h"
#include "y.tab.h"

extern FILE * yyin;
extern int yyparse();
extern Tree * getOutputTree();
extern void setLexDebugMode(bool isDebugModeOn);

static char * inputPascalFileName = 0;
static FILE * outputAssemblyFile = 0;
static bool debugModeOn = false;

int main(int argc, char *argv[]){
	outputAssemblyFile = stdout;

	if(argc > 1) {
		int i = 1;
		while (i < argc) {
			char * arg = argv[i];
			if ( strcmp(arg,"-o") == 0 ) {
				if ( outputAssemblyFile != stdout) {
					fprintf(stderr,"Command Line Option \"-o\" is already specifies a file name for the output file\n");
					exit (1);
				}
				if (argc == i) {
					fprintf(stderr,"Command Line Option \"-o\" must have corresponding file name for the output file\n");
					exit (1);
				} else {
					i++;
					if( access( argv[i], F_OK ) != -1 ) {
						if( access( argv[i], W_OK ) == -1 ) {
							fprintf(stderr,"File \"%s\" does not exist or does not have proper permissions\n", argv[i]);
							exit(1);
						}
					} 
				  	outputAssemblyFile = fopen (argv[i],"w");				
				}
			}  else if (strcmp(arg,"-d") == 0) {
				debugModeOn = true;
				setLexDebugMode(true);
			}  else {
				if (inputPascalFileName) {
					fprintf(stderr,"Extra Command Line Argument: \"%s\"\n", inputPascalFileName);
					exit (1);
				} else {
					inputPascalFileName = argv[i];
					if( access( inputPascalFileName, R_OK ) == -1 || access( inputPascalFileName, R_OK ) == -1 ) { // check file exists
						fprintf(stderr, "File \"%s\" does not exist or does not have proper permissions\n", argv[i]);
						exit(1);
					}
				}
			}
			i++;
		}
	}
		

	if (inputPascalFileName) {
		yyin = fopen( inputPascalFileName , "r" );
		if(debugModeOn)
			printf("\n\nSTART PARSING\n\n");
		yyparse();
		fclose( yyin );
	} else {
		if(debugModeOn)
			printf("\n\nSTART PARSING\n\n");
		yyparse();
	}

	Tree * tree = getOutputTree();

	if(debugModeOn) {
  		printf("\n\nDONE PARSING\n\n");

  		printf("\n\nSYNTAX TREE:\n\n");
  		treePrint(tree);

  		printf("\n\nSTART ANALYZING SEMANTICS\n\n");
  	}

  	SymbolTable * symbolTable = analyzeSemantics(tree);
  	
  	if(debugModeOn) {
  		printf("\n\nDONE ANALYZING\n\n");

  		printf("\n\nCLEAN TREE (WITH LABELING):\n\n");
  		treePrint(tree);

  		printf("\n\nSTART CODE GENERATION\n\n");
  	}

  	generateCode(outputAssemblyFile, tree, symbolTable);
  	
  	if(debugModeOn) {
	  	printf("\n\nEND CODE GENERATION\n\n");

	  	printf("\n\nCLEAN TREE (WITH RE-LABELING):\n\n");
	  	treePrint(tree);
  	}

  	if(debugModeOn)
  		printf("\n\nCOMPILATION SUCCESFUL\n");

  	// end program
  	return 0;
}
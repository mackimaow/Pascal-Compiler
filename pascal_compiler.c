#include <string.h>
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


static char * inputPascalFileName = 0;
static FILE * outputAssemblyFile = 0;


int main(int argc, char *argv[]){
	if(argc > 1) {
		int i = 1;
		while (i < argc) {
			char * arg = argv[i];
			if ( strcmp(arg,"-o") == 0 ) {
				if ( outputAssemblyFile ) {
					printf("Command Line Option \"-o\" is already specifies a file name for the output file\n");
					exit (1);
				}
				if (argc == i) {
					printf("Command Line Option \"-o\" must have corresponding file name for the output file\n");
					exit (1);
				} else {
					i++;
					if( access( argv[i], F_OK ) != -1 ) {
						if( access( argv[i], W_OK ) == -1 ) {
							printf("File \"%s\" does not exist or does not have proper permissions\n", argv[i]);
							exit(1);
						}
					} 
				  	outputAssemblyFile = fopen (argv[i],"w");				
				}
			}  else {
				if (inputPascalFileName) {
					printf("Extra Command Line Argument: \"%s\"\n", inputPascalFileName);
					exit (1);
				} else {
					inputPascalFileName = argv[i];
					printf("%s\n", argv[i]);
					if( access( inputPascalFileName, R_OK ) == -1 || access( inputPascalFileName, R_OK ) == -1 ) { // check file exists
						printf("File \"%s\" does not exist or does not have proper permissions\n", argv[i]);
						exit(1);
					}
				}
			}
			i++;
		}
	}
		

	if (inputPascalFileName) {
		yyin = fopen( inputPascalFileName , "r" );
		printf("\n\nSTART PARSING\n\n");
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

  	printf("\n\nCLEAN TREE (WITH LABELING):\n\n");
  	treePrint(tree);

  	printf("\n\nSTART CODE GENERATION\n\n");
  	char * code = generateCode(tree, symbolTable);
  	printf("\n\nEND CODE GENERATION\n\n");

  	printf("\n\nCLEAN TREE (WITH RE-LABELING):\n\n");
  	treePrint(tree);

  	if (outputAssemblyFile) {
  		fprintf (outputAssemblyFile, "%s\n", code);
  	} else {
  		printf("\n\nCODE GENERATED\n\n");
  		printf("%s\n", code);
  	}

  	printf("\n\nCOMPILATION SUCCESFUL\n");

  	// end program
  	return 0;
}
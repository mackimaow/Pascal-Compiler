#include <stdio.h>
#include <unistd.h>
#include "tree.h"
#include "parsetree.h"
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
		yyin = fopen( argv[1] , "r" );
		yyparse();
		fclose( yyin );
	} else {
		yyparse();	
	}
  	printf("\n\nDONE PARSING\n\n");
  	Tree * tree = getOutputTree();
  	treePrint(tree);
}


//#include "lex.yy.h"


int main(int argc, char * argv[]) {

	if(argc == 1) {
		printf("%s", argv[0]);
		yyin = fopen(argv[1], "r");
		yylex();
		fclose(yyin);
	}
}
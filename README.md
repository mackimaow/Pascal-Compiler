# Pascal-Compiler

Compiler Haiku:

> Compile Pascal
> Segmented fault occurred
> Compile Pascal

by
Massimiliano Cutugno
















Design Document

Lexical Analyzer
	The first stage of the compiler is the lexical analyzer. This takes the input source code and translates it into stream of tokens with their corresponding attributes. The compiler used “lex” as the lexical analyzer to tokenize the input source code. The following tokens were used to do this task:

List of tokens (terminals symbols) used for grammar
| Token Name | Token Regex | Additional information |
| --- | --- | --- |
| L_PROGRAM | program | Starts a program declaration |
| L_FUNCTION | function | Starts a function declaration |
| L_PROCEDURE | procedure | Starts a procedure declaration |
| L_IF | if | Starts a if statement |
| L_THEN | then | Used with an if statement |
| L_ELSE | else | Used with an if statement |
| L_WHILE | while | Starts a while statement |
| L_FOR | for | Starts a for statement |
| L_TO | to | Used with a for statement | 
| L_DO | do | Used with a looping statements | 
| L_BEGIN | begin | Used to start multiple statements |
| L_END | end | Used to end multiple statements |
| L_INTEGER | integer | Used to declare integer type |
| L_REAL | real | Used to declare real type |
| L_VAR | var | Used to declare a variable | 
| L_ARRAY | array | Used to declare a array variable | 
| L_OF | of | Used with a array declaration | 
| L_NOT | not | Used to negate implicit boolean types |
| L_ADD | add | Used to add two reals or two integer types |
| L_SUB | sub | Used to subtract two reals or two integer types or negate integer or real types |
| L_OR | or | Used to take the logical OR of implicit boolean types | 
| L_MULOP | [*/]|div|mod|and | These are multiply,divide,modulus, and logical AND operators (respectively) that are computed with the same precedence |
| L_RELOP | =|\<\>|\<|\<=|\>=|\> | These are relational operators (equal to, not equal to, less than, less than or equal to, greater than or equal to, greater than respectively) with the same precedence |
| L_ASSIGNOP | := | Used to assign a value to a variable | 
| L_ID | [a-zA-Z]([a-zA-Z]|[0-9])* | This is an user defined name for defining functions/procedures/programs and variables |
| L_NUM | [0-9]+(E[+-]?[0-9]+)? | This is used to represent a number |
| L_LP | ( | Represents a left parentheses |
| L_RP | ) | Represents a right parentheses |
| L_LB | [ | Represents a left bracket |
| L_RB | ] | Represents a right bracket |
| L_DD | \.\. | Used with array declarations |
| L_D | \. | Used to end a program |
| L_COM | , |  Represents a comma |
| L_SC | ; | Represents a semicolon |
| L_C | : | Represents a colon |
| spaces | [ \t]+ | Spaces are filtered out at lexical stage |
| comments | \(\*(.|\n)*\*\)|\/\/.* | Comments (single and multiple line) are filtered out at lexical stage |
| EOL | \n | Represents a new line |



Parser (Syntax Analyzer)
	After the input source code is processed by the lexical analyzer (if the input does not contain any unrecognized characters) the parser constructs a parse tree. The compiler used “yacc” to build the syntax analyzer in c. The context free grammar used for this parser is listed below as a sequence of nonterminal definitions. It is worth noting that the grammer’s starting non-terminal is “program”. Furthermore, to rid of the dangling else ambiguous grammar, two additional non-terminal types (matched_stmt and unmatched_stmt) had to be constructed.

List of  (nonterminals) used for Grammar
Non-Terminal
Non-Terminal Rule(s)
program
L_PROGRAM    L_ID    L_LP   identifier_list   L_RP     L_SC   declarations  subprogram_declarations  compound_statement  L_D
identifier_list
L_ID
|   identifier_list   L_COM   L_ID
declarations

| declarations   L_VAR  identifier_list   L_C type   L_SC	
type
standard_type
|   L_ARRAY   L_LB    L_NUM    L_DD    L_NUM    L_RB   L_OF   standard_type
standard_type
L_INTEGER
|  L_REAL
subprogram_declarations

|  subprogram_declarations   subprogram_declaration   L_SC
subprogram_declaration
subprogram_head  declarations  subprogram_declarations  compound_statement  
subprogram_head
L_FUNCTION  L_ID  arguments  L_C  standard_type  L_SC
|  L_PROCEDURE    L_ID    arguments   L_SC	
arguments

|  L_LP    parameter_list    L_RP 
parameter_list
identifier_list   L_C   type  
|  parameter_list    L_SC    identifier_list   L_C   type  
compound_statement
L_BEGIN    optional_statements    L_END 
optional_statements

|  statement_list
statement_list
statement
|  statement_list    L_SC    statement
statement
  matched_stmt
|  unmatched_stmt
matched_stmt
 L_IF expression   L_THEN   matched_stmt   L_ELSE   matched_stmt
|  variable  L_ASSIGNOP  expression
|  procedure_statement	
|  compound_statement
|  L_WHILE  expression  L_DO  statement
|  L_FOR  L_ID  L_ASSIGNOP  expression  L_TO  expression  L_DO  statement
unmatched_stmt
L_IF   expression  L_THEN   statement
|  L_IF  expression   L_THEN  matched_stmt  L_ELSE  unmatched_stmt
variable
L_ID
|  L_ID  L_LB  expression  L_RB
procedure_statement
L_ID
|  L_ID    L_LP    expression_list    L_RP
expression_list
expression
|  expression_list  L_COM  expression
expression
simple_expression 
| simple_expression  L_RELOP  simple_expression
simple_expression
term
| L_SUB   term 
| simple_expression   L_ADD  term
| simple_expression  L_SUB  term
| simple_expression  L_OR  term
term
factor
|  term   L_MULOP   factor
factor
variable
|  L_ID    L_LP   expression_list   L_RP
|  L_NUM
|  L_LP   expression   L_RP
|  L_NOT   factor 


















Semantic Analyzer
	After the constructing the parse tree from the specified grammer, the compiler then proceeds to check the semantics of the program. This is done in separate c file “semantic_analyzer.c”. The semantic analyzer will halt and exit the compilation if any of the following rules are broken:
A function must not be left without returning a value
A function returns a value and must not leave dead code behind (code that is never reached because of the function return blocks it)
The conditions for if statements and while statements must be of type boolean
A assignment of variable must have its left hand side match the right hand side when expression is computed
Assignment of a whole array to another whole array is not allowed
An array must have its upper bound be greater than its lower bound
A declaration of a variable/function/procedure in a scope must not have the same name as any other declared variables/functions/procedures within the scope (including parameters)
Within the function, any of its local functions/procedures/variables within scope should not have the same name as the the function name
A function is never allowed to access variables/procedures/functions outside of its scope (except for accessing itself for recursion)
The write procedure can take in any number of arguments, however must take in at least one argument
Functions must not ever be directly used as a statement and must be accessed where a expression is present
Procedures must not be used within a expression and must be called as a single statement
Functions/procedures/variables cannot be used if not previously declared
Functions and procedures must be called with the right arguments types and the write number of arguments
Expressions must be acted on a single type (even implicitly declared boolean types by the use of relational operators). Also note that compile time constants are casted to either a real or a integer. 
Functions, when used in a expression, must have its return type be the same as operand types also within the expression
The logical AND, OR, and NOT operations must act on implicit boolean types, while all other operations must act on the types specified by the operands.
Cannot mismatch function/procedure/variable names with another another
An array passed though as a parameter to a function is allowed, but must have the same declared upper bound and lower bound as the parameter type
NOTE: All indices of arrays are checked during runtime and not during compile time



Code Generation
	The compiler is finally ready to generate assembly source code after it successfully passes through the semantic analyzer stage with no violations of specified rules. This compiler then generates x86 assembly code (intel syntax). The code generated can be then executed after the user installs nasm (or any equivalent object compiler program that takes in x86 assembly code). There is only one expected type of error that could happen and is safely handled by stopping the execution of the program. This error occurs when the users decides to index a array which is out of the bounds of the array given. A segmented fault should never occur after executing the assembly code.

User Manual

	To compile the user source code for pascal, one should first compile the source code for the compiler using the make file within the project directory. This is done by executing the following command:
> make all
Alternatively, one could use the following command:
> make pcc
After this is executed, the pascal compiler is ready to compile the source code for pascal program. There are a few options for compiling the source code. One is directly passing the input though piping or redirecting input:
	> cat [pascal_source_code] | ./pcc
This will however print the source code to the standard output. To redirect the output to the a filename, one should use the ‘-o’ option with a output file name like below:
	> cat [pascal_source_code] | ./pcc -o [outputfileName]
Furthermore, one could just specify the input file without redirection/piping like below:
	> ./pcc [pascal_source_code] -o [outputfileName] 
Any of these ways should generate the assembly code code in one way or another. It is worth noting that piping the output of the compiler is not a sufficient way to generating source code, as the compiler will attempt to print the lexical tokens, parse tree (both semantically unchecked and clean/checked trees) alongside the output assembly code. Therefore, one should not do the following:
 > ./pcc [pascal_source_code] > [outputfileName]
or 
	 > ./pcc [pascal_source_code] | [other commands...]
To execute the assembly code, one should first install “nasm” to create a object file for the program. To do this on a ubuntu, one should first do the following:
	> sudo apt-get install nasm
After this is done, the assembly code can be executed by using the bash script “runAssembly” in the project directory. More specifically, one should do the following:
	> ./runAssembly [outputAssemblyFile]
This should run the assembly program that the pascal source code represents.
Status Report

	The pascal compiler presented in this report is not complete; there are a few limitations and caveats as well as unique features. Firstly, the parameters specified in the program definition has no functionality yet. Therefore using “input” or “output” within the program parameters effectively does not add input or output features to the pascal source code. However, it is worth noting that is still possible to use the “write” function (with one or more arguments) to print a result to standard out. Furthermore, it is not possible to read from standard in due to segmented fault errors when executing the assembly source code. The semantic analyzer will not throw any errors when the user proceeds to use the read from standard in, but the assembly code will skip over this action when executed to prevent the segmented fault. The compiler also does not handle floating point numbers, (so there is no difference between real types and integer types). However, a unique feature to this compiler is that the user is able to enter a number in scientific notation to declare a number wherever is appropriate.
  
Testing Report

	The compiler was tested against its specifications to determine if there are any overall problems to be fixed. Each section below shows a pascal source file with its output result. If a error is expected, the description will rightful mention this.


Program 1 
[Testing greatest common denominator function with 345 and 678 (should compile and run normally as expected)]

[source code]

program example(input, output);
	var x:  integer;
	function gcd(a, b: integer): integer;
	begin
	   if b = 0 then gcd := a;
	   gcd := gcd(b, a mod b)
	end;
begin
	x := gcd(345, 678);
	write(x)
end.

[output]
		3
Program 2
[Storing the function range of x^2 in a array from 3 to 10 then printing the result (should compile and run normally as expected)]

[source code]

program example(input, output);
	var x:  integer;
	var someArray : array [ 3 .. 10 ] of integer ; 
begin
	for x:= 3 to 10 do
		someArray[x] := x*x;

	for x:= 3 to 10 do
		write(someArray[x])
end.

[output]
		9
16
25
36
49
64
81
100

















Program 3
[Attempting to index a array that is out of bounds (should compile and but give a runtime error)]

[source code]

program example(input, output);
	var x:  integer;
	var someArray : array [ 3 .. 10 ] of integer ; 
begin
	x:=2;
	someArray[x] := 100
end.


[output]
		[ERROR] ARRAY OUT OF BOUNDS!
2
3


	






















Program 4
[Attempting to get a variable out of scope of a function (should semantically fail)]

[source code]

program example(input, output);
	var y:  integer;
	function bad (x : integer) : integer;
	begin
		bad := x mod y
	end;
begin
	y:=10;
	write(bad(100))
end.



[output]
		[ERROR] VARIABLE "y" was referenced at (6,16), but was never declared. [TRACE: example.bad]






















Program 5
[Attempting to get a variable out of scope of a procedure (should compile and run normally as expected)]

[source code]

program example(input, output);
	var y:  integer;
	var result : integer;
	procedure okay (x : integer);
	begin
		result := x mod y
	end;
begin
	y:=10;
	okay(123);
	write(result)
end.




[output]
		3



















Program 6
[Attempting to get a assign a variable to a different type (should fail at semantic stage)]

[source code]

program example(input, output);
	var x:  integer;
	var y:  real;
begin
	y:=3; 
	x:=2*y;
	write(x)
end.





[output]
		[ERROR] ASSIGNMENT of variable "x" of type "INTEGER" does not match left hand side type of "REAL" at (7,2). [TRACE: example]






















Program 7
[Testing greatest common denominator function with 345 and 678, but the return type of the function does not match the assigning variable (should fail at semantic stage)]

[source code]

program example(input, output);
	var x:  real;
	function gcd(a, b: integer): integer;
	begin
	   if b = 0 then gcd := a;
	   gcd := gcd(b, a mod b)
	end;
begin
	x:= gcd(345, 678);
	write(x)
end.


[output]
		[ERROR] ASSIGNMENT of variable "x" of type "REAL" does not match left hand side type of "INTEGER" at (9,2). [TRACE: example]





















Program 8
[Finding the first fibonacci number before 50 (should compile and run normally as expected)]

[source code]

program example(input, output);
	var x, y:  real;
begin
	y := 0;
	x := 1;
	while x < 50 do
	begin
		x := x + y;
		y := x - y
	end;
	write(y)
end.



[output]
		34



# Pascal-Compiler

Compiler Haiku:

> Compile Pascal  
Segmented fault occurred  
Compile Pascal

by
Massimiliano Cutugno


## Design Document

### Lexical Analyzer
The first stage of the compiler is the lexical analyzer. This takes the input source code and translates it into stream of tokens with their corresponding attributes. The compiler used “lex” as the lexical analyzer to tokenize the input source code. The following tokens were used to do this task:

List of tokens (terminals symbols) used for grammar:

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
| L_MULOP | [*/]\|div\|mod\|and | These are multiply,divide,modulus, and logical AND operators (respectively) that are computed with the same precedence |
| L_RELOP | =\|\\<\\>\|\\<\|\\<=\|\\>=\|\\> | These are relational operators (equal to, not equal to, less than, less than or equal to, greater than or equal to, greater than respectively) with the same precedence |
| L_ASSIGNOP | := | Used to assign a value to a variable |
| L_ID | \[a-zA-Z\](\[a-zA-Z\]\|\[0-9\])* | This is an user defined name for defining functions/procedures/programs and variables |
| L_NUM | \[0-9\]+(E\[+-\]?\[0-9\]+)? | This is used to represent a number |
| L_LP | ( | Represents a left parentheses |
| L_RP | ) | Represents a right parentheses |
| L_LB | \[ | Represents a left bracket |
| L_RB | \] | Represents a right bracket |
| L_DD | \\.\\. | Used with array declarations |
| L_D | \\. | Used to end a program |
| L_COM | , |  Represents a comma |
| L_SC | ; | Represents a semicolon | 
| L_C | : | Represents a colon |
| spaces | \[ \t\]+ | Spaces are filtered out at lexical stage |
| comments | \\(\\\*(.\|\\n)\*\\\*\\)\|\\/\\/.\* | Comments (single and multiple line) are filtered out at lexical stage |
| EOL | \\n | Represents a new line |

 ### Parser (Syntax Analyzer)
After the input source code is processed by the lexical analyzer (if the input does not contain any unrecognized characters) the parser constructs a parse tree. The compiler used “yacc” to build the syntax analyzer in c. The context free grammar used for this parser is listed below as a sequence of nonterminal definitions. It is worth noting that the grammer’s starting non-terminal is “program”. Furthermore, to rid of the dangling else ambiguous grammar, two additional non-terminal types (matched_stmt and unmatched_stmt) had to be constructed.

List of  (nonterminals) used for grammar:

| Non-Terminal | Non-Terminal Rule(s) |
| --- | --- |
| program | L\_PROGRAM  L\_ID  L\_LP  identifier\_list   L\_RP     L\_SC   declarations  subprogram\_declarations  compound\_statement  L\_D |
| identifier\_list | L\_ID <br> \|   identifier\_list   L\_COM   L\_ID |
| declarations | epsilon <br> \| declarations   L\_VAR  identifier\_list   L\_C type   L\_SC |	
| type | standard\_type <br> \|   L\_ARRAY   L\_LB    L\_NUM    L\_DD    L\_NUM    L\_RB   L\_OF   standard\_type |
| standard\_type | L\_INTEGER  <br>  \|  L\_REAL |
| subprogram\_declarations | epsilon <br> \|  subprogram\_declarations   subprogram\_declaration   L\_SC |
| subprogram\_declaration | subprogram\_head  declarations  subprogram\_declarations  compound\_statement |  
| subprogram\_head | L\_FUNCTION  L\_ID  arguments  L\_C  standard\_type  L\_SC  <br> \|  L\_PROCEDURE    L\_ID    arguments   L\_SC	|
| arguments | epsilon  <br> \|  L\_LP    parameter\_list    L\_RP | 
| parameter\_list | identifier\_list   L\_C   type  <br> \|  parameter\_list    L\_SC    identifier\_list   L\_C   type |  
| compound\_statement | L\_BEGIN    optional\_statements    L\_END | 
| optional\_statements | epsilon  <br> \|  statement\_list |
| statement\_list | statement  <br> \|  statement\_list    L\_SC    statement |
| statement | matched\_stmt  <br> \|  unmatched\_stmt |
| matched\_stmt | L\_IF expression   L\_THEN   matched\_stmt   L\_ELSE   matched\_stmt  <br> \|  variable  L\_ASSIGNOP  expression  <br> \|  procedure\_statement  <br> \|  compound\_statement  <br> \|  L\_WHILE  expression  L\_DO  statement  <br> \|  L\_FOR  L\_ID  L\_ASSIGNOP  expression  L\_TO  expression  L\_DO  statement |
| unmatched\_stmt | L\_IF   expression  L\_THEN   statement  <br> \|  L\_IF  expression   L\_THEN  matched\_stmt  L\_ELSE  unmatched\_stmt |
| variable | L\_ID  <br> \|  L\_ID  L\_LB  expression  L\_RB |
| procedure\_statement | L\_ID  <br> \|  L\_ID    L\_LP    expression\_list    L\_RP |
| expression\_list | expression  <br> \|  expression\_list  L\_COM  expression |
| expression | simple\_expression  <br> \| simple\_expression  L\_RELOP  simple\_expression |
| simple\_expression | term  <br> \| L\_SUB   term  <br> \| simple\_expression   L\_ADD  term  <br> \| simple\_expression  L\_SUB  term  <br> \| simple\_expression  L\_OR  term |
| term | factor  <br> \|  term   L\_MULOP   factor |
| factor | variable  <br> \|  L\_ID    L\_LP   expression\_list   L\_RP  <br> \|  L\_NUM  <br> \|  L\_LP   expression   L\_RP  <br> \|  L\_NOT   factor |

### Semantic Analyzer
After the constructing the parse tree from the specified grammer, the compiler then proceeds to check the semantics of the program. This is done in separate c file “semantic\_analyzer.c”. The semantic analyzer will halt and exit the compilation if any of the following rules are broken:
- A function must not be left without returning a value
- A function returns a value and must not leave dead code behind (code that is never reached because of the function return blocks it)
- The conditions for if statements and while statements must be of type boolean
- A assignment of variable must have its left hand side match the right hand side when expression is computed
- Assignment of a whole array to another whole array is not allowed
- An array must have its upper bound be greater than its lower bound
- A declaration of a variable/function/procedure in a scope must not have the same name as any other declared variables/functions/procedures within the scope (including parameters)
- Within the function, any of its local functions/procedures/variables within scope should not have the same name as the the function name
- A function is never allowed to access variables/procedures/functions outside of its scope (except for accessing itself for recursion)
- The write procedure can take in any number of arguments, however must take in at least one argument
- Functions must not ever be directly used as a statement and must be accessed where a expression is present
- Procedures must not be used within a expression and must be called as a single statement
- Functions/procedures/variables cannot be used if not previously declared
- Functions and procedures must be called with the right arguments types and the write number of arguments
- Expressions must be acted on a single type (even implicitly declared boolean types by the use of relational operators). Also note that compile time constants are casted to either a real or a integer. 
- Functions, when used in a expression, must have its return type be the same as operand types also within the expression
- The logical AND, OR, and NOT operations must act on implicit boolean types, while all other operations must act on the types specified by the operands.
- Cannot mismatch function/procedure/variable names with another another
- An array passed though as a parameter to a function is allowed, but must have the same declared upper bound and lower bound as the parameter type
- NOTE: All indices of arrays are checked during runtime and not during compile time

### Code Generation
The compiler is finally ready to generate assembly source code after it successfully passes through the semantic analyzer stage with no violations of specified rules. This compiler then generates x86 assembly code (intel syntax). The code generated can be then executed after the user installs nasm (or any equivalent object compiler program that takes in x86 assembly code). There is only one expected type of error that could happen and is safely handled by stopping the execution of the program. This error occurs when the users decides to index a array which is out of the bounds of the array given. A segmented fault should never occur after executing the assembly code.

The follow table shows the stack arrangement used the a procedure/function call:

 | Stack |
 | :---: |
 |  temporary register S  | 
 | : <br>: | 
 |  temporary register 1  | 
 |  local variable M  | 
 |  : <br>:  | 
 |  local variable 1  | 
 |  calling scope bsp   (current scope bsp points to this address) |
 |  return instruction address   | 
 |  parameter N  | 
 |  : <br>:  | 
 |  parameter 1  |

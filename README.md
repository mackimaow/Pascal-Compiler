# Pascal-Compiler

Compiler Haiku:

> Compile Pascal  
Segmented fault occurred  
Compile Pascal

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


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
 Token Name | Token Regex | Additional information
 --- | --- | --- 
 L_PROGRAM | program | Starts a program declaration 



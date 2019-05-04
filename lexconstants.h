#ifndef LEX_CONSTANTS_H
#define LEX_CONSTANTS_H


extern int lineNumber;
extern int lineIndex;

char * lexConstantToString(int value);
void processLexString(char * stringValue);
void lexError(char * stringValue);


#endif // LEX_CONSTANTS_H
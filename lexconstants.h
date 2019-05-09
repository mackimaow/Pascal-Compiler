#ifndef LEX_CONSTANTS_H
#define LEX_CONSTANTS_H

#include "stdbool.h"

extern int lineNumber;
extern int lineIndex;

#define LL_BOOLEAN 257

#define LL_PROGRAM 258
#define LL_FUNCTION 259
#define LL_PROCEDURE 260
#define LL_IF 261
#define LL_THEN 262
#define LL_ELSE 263
#define LL_WHILE 264
#define LL_FOR 265
#define LL_TO 266
#define LL_DO 267
#define LL_BEGIN 268
#define LL_END 269
#define LL_INTEGER 270
#define LL_REAL 271
#define LL_VAR 272
#define LL_ARRAY 273
#define LL_OF 274
#define LL_NOT 275
#define LL_ADD 276
#define LL_SUB 277
#define LL_OR 278
#define LL_MULOP 279
#define LL_RELOP 280
#define LL_ASSIGNOP 281
#define LL_ID 282
#define LL_NUM 283
#define LL_LP 284
#define LL_RP 285
#define LL_LB 286
#define LL_RB 287
#define LL_DD 288
#define LL_D 289
#define LL_COM 290
#define LL_SC 291
#define LL_C 292

char * lexConstantToString(int value);
char * lexConstantToStringNoLL(int value);
void processLexString(char * stringValue);
void lexError(char * stringValue);

#endif // LEX_CONSTANTS_H
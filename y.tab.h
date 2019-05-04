/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    L_PROGRAM = 258,
    L_FUNCTION = 259,
    L_PROCEDURE = 260,
    L_IF = 261,
    L_THEN = 262,
    L_ELSE = 263,
    L_WHILE = 264,
    L_FOR = 265,
    L_TO = 266,
    L_DO = 267,
    L_BEGIN = 268,
    L_END = 269,
    L_INTEGER = 270,
    L_REAL = 271,
    L_VAR = 272,
    L_ARRAY = 273,
    L_OF = 274,
    L_NOT = 275,
    L_ADD = 276,
    L_SUB = 277,
    L_OR = 278,
    L_MULOP = 279,
    L_RELOP = 280,
    L_ASSIGNOP = 281,
    L_ID = 282,
    L_NUM = 283,
    L_LP = 284,
    L_RP = 285,
    L_LB = 286,
    L_RB = 287,
    L_DD = 288,
    L_D = 289,
    L_COM = 290,
    L_SC = 291,
    L_C = 292
  };
#endif
/* Tokens.  */
#define L_PROGRAM 258
#define L_FUNCTION 259
#define L_PROCEDURE 260
#define L_IF 261
#define L_THEN 262
#define L_ELSE 263
#define L_WHILE 264
#define L_FOR 265
#define L_TO 266
#define L_DO 267
#define L_BEGIN 268
#define L_END 269
#define L_INTEGER 270
#define L_REAL 271
#define L_VAR 272
#define L_ARRAY 273
#define L_OF 274
#define L_NOT 275
#define L_ADD 276
#define L_SUB 277
#define L_OR 278
#define L_MULOP 279
#define L_RELOP 280
#define L_ASSIGNOP 281
#define L_ID 282
#define L_NUM 283
#define L_LP 284
#define L_RP 285
#define L_LB 286
#define L_RB 287
#define L_DD 288
#define L_D 289
#define L_COM 290
#define L_SC 291
#define L_C 292

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 20 "pascal.y" /* yacc.c:1909  */

  TreeValue * treeVal;
  Tree * tree;

#line 133 "y.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */

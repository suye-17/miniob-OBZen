/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_HOME_CHRIS_OBZEN_ZHANGHAO_2024_SRC_OBSERVER_SQL_PARSER_YACC_SQL_HPP_INCLUDED
# define YY_YY_HOME_CHRIS_OBZEN_ZHANGHAO_2024_SRC_OBSERVER_SQL_PARSER_YACC_SQL_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    SEMICOLON = 258,               /* SEMICOLON  */
    BY = 259,                      /* BY  */
    ORDER = 260,                   /* ORDER  */
    ASC = 261,                     /* ASC  */
    CREATE = 262,                  /* CREATE  */
    DROP = 263,                    /* DROP  */
    GROUP = 264,                   /* GROUP  */
    HAVING = 265,                  /* HAVING  */
    TABLE = 266,                   /* TABLE  */
    TABLES = 267,                  /* TABLES  */
    INDEX = 268,                   /* INDEX  */
    CALC = 269,                    /* CALC  */
    SELECT = 270,                  /* SELECT  */
    DESC = 271,                    /* DESC  */
    SHOW = 272,                    /* SHOW  */
    SYNC = 273,                    /* SYNC  */
    INSERT = 274,                  /* INSERT  */
    DELETE = 275,                  /* DELETE  */
    UPDATE = 276,                  /* UPDATE  */
    LBRACE = 277,                  /* LBRACE  */
    RBRACE = 278,                  /* RBRACE  */
    COMMA = 279,                   /* COMMA  */
    TRX_BEGIN = 280,               /* TRX_BEGIN  */
    TRX_COMMIT = 281,              /* TRX_COMMIT  */
    TRX_ROLLBACK = 282,            /* TRX_ROLLBACK  */
    INT_T = 283,                   /* INT_T  */
    STRING_T = 284,                /* STRING_T  */
    FLOAT_T = 285,                 /* FLOAT_T  */
    DATE_T = 286,                  /* DATE_T  */
    NULL_T = 287,                  /* NULL_T  */
    NOT = 288,                     /* NOT  */
    VECTOR_T = 289,                /* VECTOR_T  */
    HELP = 290,                    /* HELP  */
    EXIT = 291,                    /* EXIT  */
    DOT = 292,                     /* DOT  */
    INTO = 293,                    /* INTO  */
    VALUES = 294,                  /* VALUES  */
    FROM = 295,                    /* FROM  */
    WHERE = 296,                   /* WHERE  */
    AND = 297,                     /* AND  */
    SET = 298,                     /* SET  */
    ON = 299,                      /* ON  */
    LOAD = 300,                    /* LOAD  */
    DATA = 301,                    /* DATA  */
    INFILE = 302,                  /* INFILE  */
    EXPLAIN = 303,                 /* EXPLAIN  */
    STORAGE = 304,                 /* STORAGE  */
    FORMAT = 305,                  /* FORMAT  */
    PRIMARY = 306,                 /* PRIMARY  */
    KEY = 307,                     /* KEY  */
    ANALYZE = 308,                 /* ANALYZE  */
    COUNT = 309,                   /* COUNT  */
    SUM = 310,                     /* SUM  */
    AVG = 311,                     /* AVG  */
    MAX = 312,                     /* MAX  */
    MIN = 313,                     /* MIN  */
    EQ = 314,                      /* EQ  */
    LT = 315,                      /* LT  */
    GT = 316,                      /* GT  */
    LE = 317,                      /* LE  */
    GE = 318,                      /* GE  */
    NE = 319,                      /* NE  */
    NUMBER = 320,                  /* NUMBER  */
    FLOAT = 321,                   /* FLOAT  */
    ID = 322,                      /* ID  */
    SSS = 323,                     /* SSS  */
    UMINUS = 324                   /* UMINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 141 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"

  ParsedSqlNode *                            sql_node;
  ConditionSqlNode *                         condition;
  Value *                                    value;
  enum CompOp                                comp;
  RelAttrSqlNode *                           rel_attr;
  vector<AttrInfoSqlNode> *                  attr_infos;
  AttrInfoSqlNode *                          attr_info;
  Expression *                               expression;
  vector<unique_ptr<Expression>> *           expression_list;
  vector<Value> *                            value_list;
  vector<ConditionSqlNode> *                 condition_list;
  vector<RelAttrSqlNode> *                   rel_attr_list;
  vector<string> *                           relation_list;
  vector<string> *                           key_list;
  char *                                     cstring;
  int                                        number;
  float                                      floats;

#line 153 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif




int yyparse (const char * sql_string, ParsedSqlResult * sql_result, void * scanner);


#endif /* !YY_YY_HOME_CHRIS_OBZEN_ZHANGHAO_2024_SRC_OBSERVER_SQL_PARSER_YACC_SQL_HPP_INCLUDED  */

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

#ifndef YY_YY_HOME_SUYE_MINIOB_MINIOB_SRC_OBSERVER_SQL_PARSER_YACC_SQL_HPP_INCLUDED
# define YY_YY_HOME_SUYE_MINIOB_MINIOB_SRC_OBSERVER_SQL_PARSER_YACC_SQL_HPP_INCLUDED
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
    UNIQUE = 269,                  /* UNIQUE  */
    CALC = 270,                    /* CALC  */
    SELECT = 271,                  /* SELECT  */
    DESC = 272,                    /* DESC  */
    SHOW = 273,                    /* SHOW  */
    SYNC = 274,                    /* SYNC  */
    INSERT = 275,                  /* INSERT  */
    DELETE = 276,                  /* DELETE  */
    UPDATE = 277,                  /* UPDATE  */
    LBRACE = 278,                  /* LBRACE  */
    RBRACE = 279,                  /* RBRACE  */
    LSBRACE = 280,                 /* LSBRACE  */
    RSBRACE = 281,                 /* RSBRACE  */
    COMMA = 282,                   /* COMMA  */
    TRX_BEGIN = 283,               /* TRX_BEGIN  */
    TRX_COMMIT = 284,              /* TRX_COMMIT  */
    TRX_ROLLBACK = 285,            /* TRX_ROLLBACK  */
    INT_T = 286,                   /* INT_T  */
    STRING_T = 287,                /* STRING_T  */
    FLOAT_T = 288,                 /* FLOAT_T  */
    DATE_T = 289,                  /* DATE_T  */
    NULL_T = 290,                  /* NULL_T  */
    NOT = 291,                     /* NOT  */
    IS = 292,                      /* IS  */
    VECTOR_T = 293,                /* VECTOR_T  */
    TEXT_T = 294,                  /* TEXT_T  */
    HELP = 295,                    /* HELP  */
    EXIT = 296,                    /* EXIT  */
    DOT = 297,                     /* DOT  */
    INTO = 298,                    /* INTO  */
    VALUES = 299,                  /* VALUES  */
    FROM = 300,                    /* FROM  */
    WHERE = 301,                   /* WHERE  */
    AND = 302,                     /* AND  */
    SET = 303,                     /* SET  */
    ON = 304,                      /* ON  */
    LOAD = 305,                    /* LOAD  */
    DATA = 306,                    /* DATA  */
    INFILE = 307,                  /* INFILE  */
    EXPLAIN = 308,                 /* EXPLAIN  */
    STORAGE = 309,                 /* STORAGE  */
    FORMAT = 310,                  /* FORMAT  */
    PRIMARY = 311,                 /* PRIMARY  */
    KEY = 312,                     /* KEY  */
    ANALYZE = 313,                 /* ANALYZE  */
    EQ = 314,                      /* EQ  */
    LT = 315,                      /* LT  */
    GT = 316,                      /* GT  */
    LE = 317,                      /* LE  */
    GE = 318,                      /* GE  */
    NE = 319,                      /* NE  */
    L2_DISTANCE = 320,             /* L2_DISTANCE  */
    COSINE_DISTANCE = 321,         /* COSINE_DISTANCE  */
    INNER_PRODUCT = 322,           /* INNER_PRODUCT  */
    COUNT = 323,                   /* COUNT  */
    SUM = 324,                     /* SUM  */
    AVG = 325,                     /* AVG  */
    MAX = 326,                     /* MAX  */
    MIN = 327,                     /* MIN  */
    IN = 328,                      /* IN  */
    LIKE = 329,                    /* LIKE  */
    EXISTS = 330,                  /* EXISTS  */
    INNER = 331,                   /* INNER  */
    JOIN = 332,                    /* JOIN  */
    NUMBER = 333,                  /* NUMBER  */
    FLOAT = 334,                   /* FLOAT  */
    ID = 335,                      /* ID  */
    SSS = 336,                     /* SSS  */
    VECTOR_LITERAL = 337,          /* VECTOR_LITERAL  */
    UMINUS = 338                   /* UMINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 182 "/home/suye/MiniOB/miniob/src/observer/sql/parser/yacc_sql.y"

  ParsedSqlNode *                            sql_node;                // SQL节点指针
  ConditionSqlNode *                         condition;               // 条件节点指针 
  Value *                                    value;                   // 值指针
  enum CompOp                                comp;                    // 比较操作符
  RelAttrSqlNode *                           rel_attr;                // 关系属性节点
  vector<AttrInfoSqlNode> *                  attr_infos;              // 属性信息列表
  AttrInfoSqlNode *                          attr_info;               // 单个属性信息
  Expression *                               expression;              // 表达式指针
  vector<unique_ptr<Expression>> *           expression_list;         // 表达式列表
  vector<Value> *                            value_list;              // 值列表
  vector<ConditionSqlNode> *                 condition_list;          // 条件列表
  vector<RelAttrSqlNode> *                   rel_attr_list;           // 关系属性列表
  vector<string> *                           relation_list;           // 关系(表)名列表
  vector<JoinSqlNode> *                      join_list;               // JOIN列表
  vector<string> *                           key_list;                // 键列表
  UpdateList *                               update_list;             // 更新列表
  char *                                     cstring;                 // 字符串指针
  int                                        number;                  // 整数
  float                                      floats;                  // 浮点数

#line 169 "/home/suye/MiniOB/miniob/src/observer/sql/parser/yacc_sql.hpp"

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


#endif /* !YY_YY_HOME_SUYE_MINIOB_MINIOB_SRC_OBSERVER_SQL_PARSER_YACC_SQL_HPP_INCLUDED  */

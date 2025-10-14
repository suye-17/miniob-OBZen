/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 2 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/log/log.h"
#include "common/lang/string.h"
#include "sql/parser/parse_defs.h"
#include "sql/parser/yacc_sql.hpp"
#include "sql/parser/lex_sql.h"
#include "sql/expr/expression.h"

using namespace std;

string token_name(const char *sql_string, YYLTYPE *llocp)
{
  return string(sql_string + llocp->first_column, llocp->last_column - llocp->first_column + 1);
}

int yyerror(YYLTYPE *llocp, const char *sql_string, ParsedSqlResult *sql_result, yyscan_t scanner, const char *msg)
{
  unique_ptr<ParsedSqlNode> error_sql_node = make_unique<ParsedSqlNode>(SCF_ERROR);
  error_sql_node->error.error_msg = msg;
  error_sql_node->error.line = llocp->first_line;
  error_sql_node->error.column = llocp->first_column;
  sql_result->add_sql_node(std::move(error_sql_node));
  return 0;
}

ArithmeticExpr *create_arithmetic_expression(ArithmeticExpr::Type type,
                                             Expression *left,
                                             Expression *right,
                                             const char *sql_string,
                                             YYLTYPE *llocp)
{
  ArithmeticExpr *expr = new ArithmeticExpr(type, left, right);
  expr->set_name(token_name(sql_string, llocp));
  return expr;
}

UnboundAggregateExpr *create_aggregate_expression(const char *aggregate_name,
                                           Expression *child,
                                           const char *sql_string,
                                           YYLTYPE *llocp)
{
  UnboundAggregateExpr *expr = new UnboundAggregateExpr(aggregate_name, child);
  expr->set_name(token_name(sql_string, llocp));
  return expr;
}


UnboundAggregateExpr *create_aggregate_expression_multi(const char *aggregate_name,
                                                       vector<unique_ptr<Expression>> *children,
                                                       const char *sql_string,
                                                       YYLTYPE *llocp)
{
  vector<unique_ptr<Expression>> child_vec;
  if (children != nullptr) {
    child_vec = std::move(*children);
    delete children;  // 释放yacc分配的向量内存
  }
  UnboundAggregateExpr *expr = new UnboundAggregateExpr(aggregate_name, std::move(child_vec));
  expr->set_name(token_name(sql_string, llocp));
  return expr;
}

ComparisonExpr *create_comparison_expression(CompOp comp_op,
                                           Expression *left,
                                           Expression *right,
                                           const char *sql_string,
                                           YYLTYPE *llocp)
{
  ComparisonExpr *expr = new ComparisonExpr(comp_op, unique_ptr<Expression>(left), unique_ptr<Expression>(right));
  expr->set_name(token_name(sql_string, llocp));
  return expr;
}


#line 151 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "yacc_sql.hpp"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_SEMICOLON = 3,                  /* SEMICOLON  */
  YYSYMBOL_BY = 4,                         /* BY  */
  YYSYMBOL_ORDER = 5,                      /* ORDER  */
  YYSYMBOL_ASC = 6,                        /* ASC  */
  YYSYMBOL_CREATE = 7,                     /* CREATE  */
  YYSYMBOL_DROP = 8,                       /* DROP  */
  YYSYMBOL_GROUP = 9,                      /* GROUP  */
  YYSYMBOL_HAVING = 10,                    /* HAVING  */
  YYSYMBOL_TABLE = 11,                     /* TABLE  */
  YYSYMBOL_TABLES = 12,                    /* TABLES  */
  YYSYMBOL_INDEX = 13,                     /* INDEX  */
  YYSYMBOL_UNIQUE = 14,                    /* UNIQUE  */
  YYSYMBOL_CALC = 15,                      /* CALC  */
  YYSYMBOL_SELECT = 16,                    /* SELECT  */
  YYSYMBOL_DESC = 17,                      /* DESC  */
  YYSYMBOL_SHOW = 18,                      /* SHOW  */
  YYSYMBOL_SYNC = 19,                      /* SYNC  */
  YYSYMBOL_INSERT = 20,                    /* INSERT  */
  YYSYMBOL_DELETE = 21,                    /* DELETE  */
  YYSYMBOL_UPDATE = 22,                    /* UPDATE  */
  YYSYMBOL_LBRACE = 23,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 24,                    /* RBRACE  */
  YYSYMBOL_COMMA = 25,                     /* COMMA  */
  YYSYMBOL_TRX_BEGIN = 26,                 /* TRX_BEGIN  */
  YYSYMBOL_TRX_COMMIT = 27,                /* TRX_COMMIT  */
  YYSYMBOL_TRX_ROLLBACK = 28,              /* TRX_ROLLBACK  */
  YYSYMBOL_INT_T = 29,                     /* INT_T  */
  YYSYMBOL_STRING_T = 30,                  /* STRING_T  */
  YYSYMBOL_FLOAT_T = 31,                   /* FLOAT_T  */
  YYSYMBOL_DATE_T = 32,                    /* DATE_T  */
  YYSYMBOL_NULL_T = 33,                    /* NULL_T  */
  YYSYMBOL_NOT = 34,                       /* NOT  */
  YYSYMBOL_IS = 35,                        /* IS  */
  YYSYMBOL_VECTOR_T = 36,                  /* VECTOR_T  */
  YYSYMBOL_HELP = 37,                      /* HELP  */
  YYSYMBOL_EXIT = 38,                      /* EXIT  */
  YYSYMBOL_DOT = 39,                       /* DOT  */
  YYSYMBOL_INTO = 40,                      /* INTO  */
  YYSYMBOL_VALUES = 41,                    /* VALUES  */
  YYSYMBOL_FROM = 42,                      /* FROM  */
  YYSYMBOL_WHERE = 43,                     /* WHERE  */
  YYSYMBOL_AND = 44,                       /* AND  */
  YYSYMBOL_SET = 45,                       /* SET  */
  YYSYMBOL_ON = 46,                        /* ON  */
  YYSYMBOL_LOAD = 47,                      /* LOAD  */
  YYSYMBOL_DATA = 48,                      /* DATA  */
  YYSYMBOL_INFILE = 49,                    /* INFILE  */
  YYSYMBOL_EXPLAIN = 50,                   /* EXPLAIN  */
  YYSYMBOL_STORAGE = 51,                   /* STORAGE  */
  YYSYMBOL_FORMAT = 52,                    /* FORMAT  */
  YYSYMBOL_PRIMARY = 53,                   /* PRIMARY  */
  YYSYMBOL_KEY = 54,                       /* KEY  */
  YYSYMBOL_ANALYZE = 55,                   /* ANALYZE  */
  YYSYMBOL_COUNT = 56,                     /* COUNT  */
  YYSYMBOL_SUM = 57,                       /* SUM  */
  YYSYMBOL_AVG = 58,                       /* AVG  */
  YYSYMBOL_MAX = 59,                       /* MAX  */
  YYSYMBOL_MIN = 60,                       /* MIN  */
  YYSYMBOL_IN = 61,                        /* IN  */
  YYSYMBOL_LIKE = 62,                      /* LIKE  */
  YYSYMBOL_EXISTS = 63,                    /* EXISTS  */
  YYSYMBOL_INNER = 64,                     /* INNER  */
  YYSYMBOL_JOIN = 65,                      /* JOIN  */
  YYSYMBOL_NUMBER = 66,                    /* NUMBER  */
  YYSYMBOL_FLOAT = 67,                     /* FLOAT  */
  YYSYMBOL_ID = 68,                        /* ID  */
  YYSYMBOL_SSS = 69,                       /* SSS  */
  YYSYMBOL_70_ = 70,                       /* '+'  */
  YYSYMBOL_71_ = 71,                       /* '-'  */
  YYSYMBOL_72_ = 72,                       /* '*'  */
  YYSYMBOL_73_ = 73,                       /* '/'  */
  YYSYMBOL_EQ = 74,                        /* EQ  */
  YYSYMBOL_NE = 75,                        /* NE  */
  YYSYMBOL_LT = 76,                        /* LT  */
  YYSYMBOL_LE = 77,                        /* LE  */
  YYSYMBOL_GT = 78,                        /* GT  */
  YYSYMBOL_GE = 79,                        /* GE  */
  YYSYMBOL_UMINUS = 80,                    /* UMINUS  */
  YYSYMBOL_YYACCEPT = 81,                  /* $accept  */
  YYSYMBOL_commands = 82,                  /* commands  */
  YYSYMBOL_command_wrapper = 83,           /* command_wrapper  */
  YYSYMBOL_exit_stmt = 84,                 /* exit_stmt  */
  YYSYMBOL_help_stmt = 85,                 /* help_stmt  */
  YYSYMBOL_sync_stmt = 86,                 /* sync_stmt  */
  YYSYMBOL_begin_stmt = 87,                /* begin_stmt  */
  YYSYMBOL_commit_stmt = 88,               /* commit_stmt  */
  YYSYMBOL_rollback_stmt = 89,             /* rollback_stmt  */
  YYSYMBOL_drop_table_stmt = 90,           /* drop_table_stmt  */
  YYSYMBOL_analyze_table_stmt = 91,        /* analyze_table_stmt  */
  YYSYMBOL_show_tables_stmt = 92,          /* show_tables_stmt  */
  YYSYMBOL_desc_table_stmt = 93,           /* desc_table_stmt  */
  YYSYMBOL_create_index_stmt = 94,         /* create_index_stmt  */
  YYSYMBOL_attribute_name_list = 95,       /* attribute_name_list  */
  YYSYMBOL_drop_index_stmt = 96,           /* drop_index_stmt  */
  YYSYMBOL_show_index_stmt = 97,           /* show_index_stmt  */
  YYSYMBOL_create_table_stmt = 98,         /* create_table_stmt  */
  YYSYMBOL_attr_def_list = 99,             /* attr_def_list  */
  YYSYMBOL_attr_def = 100,                 /* attr_def  */
  YYSYMBOL_nullable_spec = 101,            /* nullable_spec  */
  YYSYMBOL_number = 102,                   /* number  */
  YYSYMBOL_type = 103,                     /* type  */
  YYSYMBOL_primary_key = 104,              /* primary_key  */
  YYSYMBOL_attr_list = 105,                /* attr_list  */
  YYSYMBOL_insert_stmt = 106,              /* insert_stmt  */
  YYSYMBOL_value_list = 107,               /* value_list  */
  YYSYMBOL_value = 108,                    /* value  */
  YYSYMBOL_storage_format = 109,           /* storage_format  */
  YYSYMBOL_delete_stmt = 110,              /* delete_stmt  */
  YYSYMBOL_update_stmt = 111,              /* update_stmt  */
  YYSYMBOL_update_list = 112,              /* update_list  */
  YYSYMBOL_select_stmt = 113,              /* select_stmt  */
  YYSYMBOL_calc_stmt = 114,                /* calc_stmt  */
  YYSYMBOL_expression_list = 115,          /* expression_list  */
  YYSYMBOL_expression = 116,               /* expression  */
  YYSYMBOL_rel_attr = 117,                 /* rel_attr  */
  YYSYMBOL_relation = 118,                 /* relation  */
  YYSYMBOL_rel_list = 119,                 /* rel_list  */
  YYSYMBOL_where = 120,                    /* where  */
  YYSYMBOL_having = 121,                   /* having  */
  YYSYMBOL_condition_list = 122,           /* condition_list  */
  YYSYMBOL_condition = 123,                /* condition  */
  YYSYMBOL_comp_op = 124,                  /* comp_op  */
  YYSYMBOL_group_by = 125,                 /* group_by  */
  YYSYMBOL_load_data_stmt = 126,           /* load_data_stmt  */
  YYSYMBOL_explain_stmt = 127,             /* explain_stmt  */
  YYSYMBOL_set_variable_stmt = 128,        /* set_variable_stmt  */
  YYSYMBOL_opt_semicolon = 129             /* opt_semicolon  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  77
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   352

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  81
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  49
/* YYNRULES -- Number of rules.  */
#define YYNRULES  139
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  285

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   331


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,    72,    70,     2,    71,     2,    73,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    74,    75,    76,    77,    78,
      79,    80
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   256,   256,   264,   265,   266,   267,   268,   269,   270,
     271,   272,   273,   274,   275,   276,   277,   278,   279,   280,
     281,   282,   283,   284,   285,   289,   295,   300,   306,   312,
     318,   324,   330,   337,   343,   350,   360,   372,   377,   384,
     392,   399,   420,   426,   435,   443,   451,   459,   470,   471,
     472,   476,   479,   480,   481,   482,   483,   487,   490,   497,
     501,   513,   523,   529,   536,   540,   544,   549,   558,   561,
     568,   579,   595,   602,   611,   639,   653,   664,   673,   678,
     689,   692,   695,   698,   702,   706,   709,   714,   720,   723,
     726,   729,   732,   735,   738,   741,   744,   747,   750,   756,
     760,   768,   773,   777,   790,   793,   799,   802,   808,   811,
     816,   827,   847,   860,   873,   886,   899,   911,   923,   935,
     947,   960,   973,   984,   995,  1008,  1024,  1025,  1026,  1027,
    1028,  1029,  1030,  1037,  1040,  1046,  1058,  1066,  1075,  1076
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "SEMICOLON", "BY",
  "ORDER", "ASC", "CREATE", "DROP", "GROUP", "HAVING", "TABLE", "TABLES",
  "INDEX", "UNIQUE", "CALC", "SELECT", "DESC", "SHOW", "SYNC", "INSERT",
  "DELETE", "UPDATE", "LBRACE", "RBRACE", "COMMA", "TRX_BEGIN",
  "TRX_COMMIT", "TRX_ROLLBACK", "INT_T", "STRING_T", "FLOAT_T", "DATE_T",
  "NULL_T", "NOT", "IS", "VECTOR_T", "HELP", "EXIT", "DOT", "INTO",
  "VALUES", "FROM", "WHERE", "AND", "SET", "ON", "LOAD", "DATA", "INFILE",
  "EXPLAIN", "STORAGE", "FORMAT", "PRIMARY", "KEY", "ANALYZE", "COUNT",
  "SUM", "AVG", "MAX", "MIN", "IN", "LIKE", "EXISTS", "INNER", "JOIN",
  "NUMBER", "FLOAT", "ID", "SSS", "'+'", "'-'", "'*'", "'/'", "EQ", "NE",
  "LT", "LE", "GT", "GE", "UMINUS", "$accept", "commands",
  "command_wrapper", "exit_stmt", "help_stmt", "sync_stmt", "begin_stmt",
  "commit_stmt", "rollback_stmt", "drop_table_stmt", "analyze_table_stmt",
  "show_tables_stmt", "desc_table_stmt", "create_index_stmt",
  "attribute_name_list", "drop_index_stmt", "show_index_stmt",
  "create_table_stmt", "attr_def_list", "attr_def", "nullable_spec",
  "number", "type", "primary_key", "attr_list", "insert_stmt",
  "value_list", "value", "storage_format", "delete_stmt", "update_stmt",
  "update_list", "select_stmt", "calc_stmt", "expression_list",
  "expression", "rel_attr", "relation", "rel_list", "where", "having",
  "condition_list", "condition", "comp_op", "group_by", "load_data_stmt",
  "explain_stmt", "set_variable_stmt", "opt_semicolon", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-203)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     230,    77,    15,   122,   122,   -57,     2,  -203,   -17,   -24,
       8,  -203,  -203,  -203,  -203,  -203,    13,    45,   230,    88,
     113,   130,  -203,  -203,  -203,  -203,  -203,  -203,  -203,  -203,
    -203,  -203,  -203,  -203,  -203,  -203,  -203,  -203,  -203,  -203,
    -203,  -203,  -203,  -203,    50,    58,   124,    78,    82,   122,
    -203,   131,   144,   154,   169,   174,  -203,  -203,   164,  -203,
     122,  -203,  -203,  -203,   147,  -203,   -11,  -203,  -203,   156,
     136,   137,   161,   134,   158,  -203,   145,  -203,  -203,  -203,
     191,   170,   157,  -203,   181,    35,   122,   122,   122,   122,
     122,   160,  -203,   122,   122,   122,   122,   122,   165,   102,
     166,   194,   172,   168,   -31,   184,  -203,   187,   192,   193,
     195,  -203,   216,    44,   235,   139,   237,   151,   238,   159,
     240,   171,  -203,  -203,   -25,   -25,  -203,  -203,  -203,   244,
     172,     6,   202,   247,   -28,   231,   220,  -203,   227,  -203,
     249,   102,  -203,   199,   -22,  -203,   236,    91,   253,  -203,
     256,   215,  -203,  -203,  -203,  -203,  -203,  -203,  -203,  -203,
    -203,  -203,  -203,   165,   275,   268,   277,   295,   251,   290,
      37,  -203,  -203,  -203,  -203,  -203,  -203,  -203,   122,   254,
     291,   133,   102,   -31,  -203,   122,   248,  -203,   306,  -203,
    -203,  -203,  -203,  -203,     7,   -43,   294,   252,   296,  -203,
     317,   312,   212,   295,   299,   301,   -31,  -203,   292,    39,
     303,    -9,   295,  -203,  -203,    55,  -203,    39,   255,   259,
     262,  -203,   297,  -203,   278,  -203,   280,  -203,    79,   252,
     122,   102,  -203,   265,   310,  -203,   -31,   105,  -203,    -9,
     107,   311,   313,  -203,   -31,   122,  -203,  -203,   314,  -203,
     316,   284,  -203,  -203,   272,   115,  -203,  -203,  -203,  -203,
     118,  -203,   123,   318,  -203,  -203,  -203,  -203,    39,   152,
     273,   269,  -203,  -203,  -203,  -203,  -203,  -203,   319,   321,
     279,   273,  -203,  -203,  -203
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,    27,     0,     0,
       0,    28,    29,    30,    26,    25,     0,     0,     0,     0,
       0,   138,    24,    23,    16,    17,    18,    19,     9,    10,
      11,    13,    14,    15,    12,     8,     5,     7,     6,     4,
       3,    20,    21,    22,     0,     0,     0,     0,     0,     0,
      67,     0,     0,     0,     0,     0,    64,    65,    99,    66,
       0,    88,    86,    77,    78,    87,    76,    34,    33,     0,
       0,     0,     0,     0,     0,   136,     0,     1,   139,     2,
       0,     0,     0,    31,     0,     0,     0,     0,     0,     0,
       0,     0,    85,     0,     0,     0,     0,     0,     0,   108,
       0,     0,   104,     0,     0,     0,    32,     0,     0,     0,
       0,    84,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   100,    79,    80,    81,    82,    83,   101,   102,
     104,     0,     0,     0,    86,   115,    87,    75,   109,    40,
       0,   108,    70,     0,   104,   137,     0,     0,    57,    42,
       0,     0,    39,    90,    89,    92,    91,    94,    93,    96,
      95,    98,    97,     0,   133,     0,     0,     0,     0,     0,
       0,   132,   126,   131,   127,   129,   128,   130,     0,     0,
       0,     0,   108,     0,   105,     0,     0,    71,     0,    52,
      53,    54,    56,    55,    47,     0,     0,     0,     0,   103,
       0,   106,     0,     0,     0,     0,     0,   113,     0,   112,
       0,     0,     0,   111,   110,     0,    62,    72,     0,     0,
       0,    49,     0,    45,     0,    43,    68,    37,     0,     0,
       0,   108,    74,     0,     0,   122,     0,     0,   114,     0,
       0,     0,     0,    61,     0,     0,   135,    51,     0,    50,
       0,     0,    41,    35,     0,     0,   134,   107,   124,   123,
       0,   118,     0,     0,   116,   120,   125,    63,    73,    46,
       0,     0,    38,    36,   119,   117,   121,    44,    59,     0,
       0,     0,    58,    69,    60
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -203,  -203,   328,  -203,  -203,  -203,  -203,  -203,  -203,  -203,
    -203,  -203,  -203,  -203,   119,  -203,  -203,  -203,  -203,   155,
      80,  -203,  -203,  -203,    70,  -203,  -202,   -87,  -203,  -203,
    -203,  -203,  -111,  -203,    -4,   -44,   -80,  -203,   189,  -117,
    -203,  -133,  -203,  -135,  -203,  -203,  -203,  -203,  -203
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,   228,    33,    34,    35,   148,   149,
     223,   248,   194,   196,   279,    36,   215,    62,   252,    37,
      38,   144,    39,    40,    63,    64,    65,   129,   130,   142,
     232,   137,   138,   178,   201,    41,    42,    43,    79
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      66,   181,    50,   186,   237,    85,   168,     4,   184,   240,
     224,    67,   134,   164,    68,    69,    92,   145,    71,   136,
     165,   141,     4,    70,    50,   147,    47,   187,    48,    49,
     220,    98,    99,   169,   260,    56,    57,   262,    59,    50,
     221,   222,   113,   115,   117,   119,   121,    96,    97,   214,
     124,   125,   126,   127,   134,   135,   204,    56,    57,   111,
      59,   136,    51,    52,    53,    54,    55,   233,   154,    93,
     207,   208,    56,    57,    58,    59,    72,    60,    61,   243,
     244,    73,   112,   114,   116,   118,   120,    85,    44,   123,
      45,    46,   234,    74,   213,   134,   216,   135,   257,    76,
     241,   242,   136,   253,   254,    94,    95,    96,    97,    94,
      95,    96,    97,    77,    94,    95,    96,    97,    80,   216,
     189,   190,   191,   192,   216,   131,    81,   193,   263,   261,
     244,   264,   244,    78,   209,    50,   132,    82,   135,   273,
     254,   217,   274,   244,   134,    49,    83,   275,   244,   216,
      84,   136,   216,   258,    86,    50,   212,   267,    51,    52,
      53,    54,    55,   156,    93,   133,    50,    87,    56,    57,
      58,    59,    93,    60,    61,   158,    93,    88,    51,    52,
      53,    54,    55,   160,    93,   221,   222,   135,    56,    57,
      58,    59,    89,    60,    61,   162,    93,    90,   100,    56,
      57,   268,    59,    91,   101,   102,   103,   105,   104,    94,
      95,    96,    97,   106,   107,   141,   108,    94,    95,    96,
      97,    94,    95,    96,    97,   109,   256,   110,   122,    94,
      95,    96,    97,   128,   139,   140,   143,     1,     2,   151,
     153,    94,    95,    96,    97,     3,     4,     5,     6,     7,
       8,     9,    10,   146,   179,   147,    11,    12,    13,   155,
     150,   157,   159,   152,   161,   166,   170,    14,    15,   163,
     167,   182,   183,   185,   171,    16,   188,    17,   195,   197,
      18,   180,   171,   198,   200,    19,   172,   173,   174,   175,
     176,   177,   202,   171,   172,   173,   174,   175,   176,   177,
     203,    94,    95,    96,    97,   172,   173,   174,   175,   176,
     177,     4,   205,   206,   211,   210,   218,   219,   226,   229,
     227,   230,   231,   235,   236,   238,   239,   246,   247,   245,
     249,   251,   250,    58,   259,   265,   271,   266,   269,   270,
     272,   278,   276,   280,   281,   282,    75,   283,   255,   277,
     225,   284,   199
};

static const yytype_int16 yycheck[] =
{
       4,   136,    33,    25,   206,    49,    34,    16,   141,   211,
      53,    68,    99,   130,    12,    13,    60,   104,    42,    99,
     131,    43,    16,    40,    33,    68,    11,   144,    13,    23,
      23,    42,    43,    61,   236,    66,    67,   239,    69,    33,
      33,    34,    86,    87,    88,    89,    90,    72,    73,   182,
      94,    95,    96,    97,   141,    99,   167,    66,    67,    24,
      69,   141,    56,    57,    58,    59,    60,   202,    24,    25,
      33,    34,    66,    67,    68,    69,    68,    71,    72,    24,
      25,    68,    86,    87,    88,    89,    90,   131,    11,    93,
      13,    14,   203,    48,   181,   182,   183,   141,   231,    11,
     211,   212,   182,    24,    25,    70,    71,    72,    73,    70,
      71,    72,    73,     0,    70,    71,    72,    73,    68,   206,
      29,    30,    31,    32,   211,    23,    68,    36,   239,    24,
      25,    24,    25,     3,   178,    33,    34,    13,   182,    24,
      25,   185,    24,    25,   231,    23,    68,    24,    25,   236,
      68,   231,   239,   233,    23,    33,    23,   244,    56,    57,
      58,    59,    60,    24,    25,    63,    33,    23,    66,    67,
      68,    69,    25,    71,    72,    24,    25,    23,    56,    57,
      58,    59,    60,    24,    25,    33,    34,   231,    66,    67,
      68,    69,    23,    71,    72,    24,    25,    23,    42,    66,
      67,   245,    69,    39,    68,    68,    45,    49,    74,    70,
      71,    72,    73,    68,    23,    43,    46,    70,    71,    72,
      73,    70,    71,    72,    73,    68,   230,    46,    68,    70,
      71,    72,    73,    68,    68,    41,    68,     7,     8,    46,
      24,    70,    71,    72,    73,    15,    16,    17,    18,    19,
      20,    21,    22,    69,    34,    68,    26,    27,    28,    24,
      68,    24,    24,    68,    24,    63,    35,    37,    38,    25,
      23,    44,    23,    74,    62,    45,    40,    47,    25,    23,
      50,    61,    62,    68,     9,    55,    74,    75,    76,    77,
      78,    79,    24,    62,    74,    75,    76,    77,    78,    79,
      23,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    16,    61,    23,    23,    61,    68,    11,    24,    23,
      68,     4,    10,    24,    23,    33,    23,    68,    66,    74,
      33,    51,    54,    68,    24,    24,    52,    24,    24,    23,
      68,    68,    24,    74,    25,    24,    18,    68,   229,   269,
     195,   281,   163
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     7,     8,    15,    16,    17,    18,    19,    20,    21,
      22,    26,    27,    28,    37,    38,    45,    47,    50,    55,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    96,    97,    98,   106,   110,   111,   113,
     114,   126,   127,   128,    11,    13,    14,    11,    13,    23,
      33,    56,    57,    58,    59,    60,    66,    67,    68,    69,
      71,    72,   108,   115,   116,   117,   115,    68,    12,    13,
      40,    42,    68,    68,    48,    83,    11,     0,     3,   129,
      68,    68,    13,    68,    68,   116,    23,    23,    23,    23,
      23,    39,   116,    25,    70,    71,    72,    73,    42,    43,
      42,    68,    68,    45,    74,    49,    68,    23,    46,    68,
      46,    24,   115,   116,   115,   116,   115,   116,   115,   116,
     115,   116,    68,   115,   116,   116,   116,   116,    68,   118,
     119,    23,    34,    63,   108,   116,   117,   122,   123,    68,
      41,    43,   120,    68,   112,   108,    69,    68,    99,   100,
      68,    46,    68,    24,    24,    24,    24,    24,    24,    24,
      24,    24,    24,    25,   120,   113,    63,    23,    34,    61,
      35,    62,    74,    75,    76,    77,    78,    79,   124,    34,
      61,   124,    44,    23,   122,    74,    25,   120,    40,    29,
      30,    31,    32,    36,   103,    25,   104,    23,    68,   119,
       9,   125,    24,    23,   113,    61,    23,    33,    34,   116,
      61,    23,    23,   108,   122,   107,   108,   116,    68,    11,
      23,    33,    34,   101,    53,   100,    24,    68,    95,    23,
       4,    10,   121,   124,   113,    24,    23,   107,    33,    23,
     107,   113,   113,    24,    25,    74,    68,    66,   102,    33,
      54,    51,   109,    24,    25,    95,   115,   122,   117,    24,
     107,    24,   107,   113,    24,    24,    24,   108,   116,    24,
      23,    52,    68,    24,    24,    24,    24,   101,    68,   105,
      74,    25,    24,    68,   105
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    81,    82,    83,    83,    83,    83,    83,    83,    83,
      83,    83,    83,    83,    83,    83,    83,    83,    83,    83,
      83,    83,    83,    83,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    94,    95,    95,    96,
      97,    98,    99,    99,   100,   100,   100,   100,   101,   101,
     101,   102,   103,   103,   103,   103,   103,   104,   104,   105,
     105,   106,   107,   107,   108,   108,   108,   108,   109,   109,
     110,   111,   112,   112,   113,   113,   113,   114,   115,   115,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   117,
     117,   118,   119,   119,   120,   120,   121,   121,   122,   122,
     122,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   124,   124,   124,   124,
     124,   124,   124,   125,   125,   126,   127,   128,   129,   129
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     2,     2,     8,     9,     1,     3,     5,
       4,     8,     1,     3,     6,     3,     5,     2,     0,     1,
       2,     1,     1,     1,     1,     1,     1,     0,     6,     1,
       3,     7,     1,     3,     1,     1,     1,     1,     0,     4,
       4,     5,     3,     5,     7,     4,     2,     2,     1,     3,
       3,     3,     3,     3,     3,     2,     1,     1,     1,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     1,
       3,     1,     1,     3,     0,     2,     0,     2,     0,     1,
       3,     3,     3,     3,     4,     1,     5,     6,     5,     6,
       5,     6,     4,     5,     5,     5,     1,     1,     1,     1,
       1,     1,     1,     0,     3,     7,     2,     4,     0,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (&yylloc, sql_string, sql_result, scanner, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location, sql_string, sql_result, scanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  YY_USE (sql_string);
  YY_USE (sql_result);
  YY_USE (scanner);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp, sql_string, sql_result, scanner);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule, const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]), sql_string, sql_result, scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, sql_string, sql_result, scanner); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
  YYLTYPE *yylloc;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;
      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  YY_USE (sql_string);
  YY_USE (sql_result);
  YY_USE (scanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yykind)
    {
    case YYSYMBOL_attribute_name_list: /* attribute_name_list  */
#line 191 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).key_list); }
#line 1587 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_attr_def_list: /* attr_def_list  */
#line 183 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).attr_infos); }
#line 1593 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_attr_def: /* attr_def  */
#line 184 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).attr_info); }
#line 1599 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_primary_key: /* primary_key  */
#line 191 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).key_list); }
#line 1605 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_attr_list: /* attr_list  */
#line 191 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).key_list); }
#line 1611 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_value_list: /* value_list  */
#line 187 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).value_list); }
#line 1617 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_value: /* value  */
#line 181 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).value); }
#line 1623 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_update_list: /* update_list  */
#line 192 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).update_list); }
#line 1629 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_expression_list: /* expression_list  */
#line 186 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).expression_list); }
#line 1635 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_expression: /* expression  */
#line 185 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).expression); }
#line 1641 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_rel_attr: /* rel_attr  */
#line 182 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).rel_attr); }
#line 1647 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_rel_list: /* rel_list  */
#line 190 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).relation_list); }
#line 1653 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_where: /* where  */
#line 188 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).condition_list); }
#line 1659 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_having: /* having  */
#line 188 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).condition_list); }
#line 1665 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_condition_list: /* condition_list  */
#line 188 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).condition_list); }
#line 1671 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_condition: /* condition  */
#line 180 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).condition); }
#line 1677 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_group_by: /* group_by  */
#line 186 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).expression_list); }
#line 1683 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, &yylloc, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* commands: command_wrapper opt_semicolon  */
#line 257 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
  {
    unique_ptr<ParsedSqlNode> sql_node = unique_ptr<ParsedSqlNode>((yyvsp[-1].sql_node));
    sql_result->add_sql_node(std::move(sql_node));
  }
#line 1992 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 25: /* exit_stmt: EXIT  */
#line 289 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
         {
      (void)yynerrs;  // 这么写为了消除yynerrs未使用的告警。如果你有更好的方法欢迎提PR
      (yyval.sql_node) = new ParsedSqlNode(SCF_EXIT);
    }
#line 2001 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 26: /* help_stmt: HELP  */
#line 295 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
         {
      (yyval.sql_node) = new ParsedSqlNode(SCF_HELP);
    }
#line 2009 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 27: /* sync_stmt: SYNC  */
#line 300 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
         {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SYNC);
    }
#line 2017 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 28: /* begin_stmt: TRX_BEGIN  */
#line 306 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
               {
      (yyval.sql_node) = new ParsedSqlNode(SCF_BEGIN);
    }
#line 2025 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 29: /* commit_stmt: TRX_COMMIT  */
#line 312 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
               {
      (yyval.sql_node) = new ParsedSqlNode(SCF_COMMIT);
    }
#line 2033 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 30: /* rollback_stmt: TRX_ROLLBACK  */
#line 318 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                  {
      (yyval.sql_node) = new ParsedSqlNode(SCF_ROLLBACK);
    }
#line 2041 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 31: /* drop_table_stmt: DROP TABLE ID  */
#line 324 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                  {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DROP_TABLE);
      (yyval.sql_node)->drop_table.relation_name = (yyvsp[0].cstring);
    }
#line 2050 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 32: /* analyze_table_stmt: ANALYZE TABLE ID  */
#line 330 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                     {
      (yyval.sql_node) = new ParsedSqlNode(SCF_ANALYZE_TABLE);
      (yyval.sql_node)->analyze_table.relation_name = (yyvsp[0].cstring);
    }
#line 2059 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 33: /* show_tables_stmt: SHOW TABLES  */
#line 337 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SHOW_TABLES);
    }
#line 2067 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 34: /* desc_table_stmt: DESC ID  */
#line 343 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
             {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DESC_TABLE);
      (yyval.sql_node)->desc_table.relation_name = (yyvsp[0].cstring);
    }
#line 2076 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 35: /* create_index_stmt: CREATE INDEX ID ON ID LBRACE attribute_name_list RBRACE  */
#line 351 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_INDEX);
      CreateIndexSqlNode &create_index = (yyval.sql_node)->create_index;
      create_index.index_name = (yyvsp[-5].cstring);
      create_index.relation_name = (yyvsp[-3].cstring);
      create_index.is_unique = false;
      create_index.attribute_names = std::move (*(yyvsp[-1].key_list));
      delete (yyvsp[-1].key_list);
    }
#line 2090 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 36: /* create_index_stmt: CREATE UNIQUE INDEX ID ON ID LBRACE attribute_name_list RBRACE  */
#line 361 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_INDEX);
      CreateIndexSqlNode &create_index = (yyval.sql_node)->create_index;
      create_index.index_name = (yyvsp[-5].cstring);
      create_index.relation_name = (yyvsp[-3].cstring);
      create_index.is_unique = true;
      create_index.attribute_names = std::move (*(yyvsp[-1].key_list));
      delete (yyvsp[-1].key_list);
    }
#line 2104 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 37: /* attribute_name_list: ID  */
#line 373 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.key_list) = new vector<string> ();
      (yyval.key_list)->push_back((yyvsp[0].cstring));
    }
#line 2113 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 38: /* attribute_name_list: attribute_name_list COMMA ID  */
#line 378 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.key_list) = (yyvsp[-2].key_list);
      (yyval.key_list)->push_back((yyvsp[0].cstring));
    }
#line 2122 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 39: /* drop_index_stmt: DROP INDEX ID ON ID  */
#line 385 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DROP_INDEX);
      (yyval.sql_node)->drop_index.index_name = (yyvsp[-2].cstring);
      (yyval.sql_node)->drop_index.relation_name = (yyvsp[0].cstring);
    }
#line 2132 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 40: /* show_index_stmt: SHOW INDEX FROM ID  */
#line 393 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SHOW_INDEX);
      (yyval.sql_node)->show_index.relation_name = (yyvsp[0].cstring);  
    }
#line 2141 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 41: /* create_table_stmt: CREATE TABLE ID LBRACE attr_def_list primary_key RBRACE storage_format  */
#line 400 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_TABLE);
      CreateTableSqlNode &create_table = (yyval.sql_node)->create_table;
      create_table.relation_name = (yyvsp[-5].cstring);
      //free($3);

      create_table.attr_infos.swap(*(yyvsp[-3].attr_infos));
      delete (yyvsp[-3].attr_infos);

      if ((yyvsp[-2].key_list) != nullptr) {
        create_table.primary_keys.swap(*(yyvsp[-2].key_list));
        delete (yyvsp[-2].key_list);
      }
      if ((yyvsp[0].cstring) != nullptr) {
        create_table.storage_format = (yyvsp[0].cstring);
      }
    }
#line 2163 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 42: /* attr_def_list: attr_def  */
#line 421 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.attr_infos) = new vector<AttrInfoSqlNode>;
      (yyval.attr_infos)->emplace_back(*(yyvsp[0].attr_info));
      delete (yyvsp[0].attr_info);
    }
#line 2173 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 43: /* attr_def_list: attr_def_list COMMA attr_def  */
#line 427 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.attr_infos) = (yyvsp[-2].attr_infos);
      (yyval.attr_infos)->emplace_back(*(yyvsp[0].attr_info));
      delete (yyvsp[0].attr_info);
    }
#line 2183 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 44: /* attr_def: ID type LBRACE number RBRACE nullable_spec  */
#line 436 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-4].number);
      (yyval.attr_info)->name = (yyvsp[-5].cstring);
      (yyval.attr_info)->length = (yyvsp[-2].number);
      (yyval.attr_info)->nullable = ((yyvsp[0].number) == 1);
    }
#line 2195 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 45: /* attr_def: ID type nullable_spec  */
#line 444 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-1].number);
      (yyval.attr_info)->name = (yyvsp[-2].cstring);
      (yyval.attr_info)->length = 4;
      (yyval.attr_info)->nullable = ((yyvsp[0].number) == 1);
    }
#line 2207 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 46: /* attr_def: ID type LBRACE number RBRACE  */
#line 452 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-3].number);
      (yyval.attr_info)->name = (yyvsp[-4].cstring);
      (yyval.attr_info)->length = (yyvsp[-1].number);
      (yyval.attr_info)->nullable = true;  
    }
#line 2219 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 47: /* attr_def: ID type  */
#line 460 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[0].number);
      (yyval.attr_info)->name = (yyvsp[-1].cstring);
      (yyval.attr_info)->length = 4;
      (yyval.attr_info)->nullable = true;  
    }
#line 2231 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 48: /* nullable_spec: %empty  */
#line 470 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                        { (yyval.number) = 1; }
#line 2237 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 49: /* nullable_spec: NULL_T  */
#line 471 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                        { (yyval.number) = 1; }
#line 2243 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 50: /* nullable_spec: NOT NULL_T  */
#line 472 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                        { (yyval.number) = 0; }
#line 2249 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 51: /* number: NUMBER  */
#line 476 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
           {(yyval.number) = (yyvsp[0].number);}
#line 2255 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 52: /* type: INT_T  */
#line 479 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
               { (yyval.number) = static_cast<int>(AttrType::INTS); }
#line 2261 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 53: /* type: STRING_T  */
#line 480 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
               { (yyval.number) = static_cast<int>(AttrType::CHARS); }
#line 2267 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 54: /* type: FLOAT_T  */
#line 481 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
               { (yyval.number) = static_cast<int>(AttrType::FLOATS); }
#line 2273 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 55: /* type: VECTOR_T  */
#line 482 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
               { (yyval.number) = static_cast<int>(AttrType::VECTORS); }
#line 2279 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 56: /* type: DATE_T  */
#line 483 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
             { (yyval.number) = static_cast<int>(AttrType::DATES); }
#line 2285 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 57: /* primary_key: %empty  */
#line 487 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.key_list) = nullptr;
    }
#line 2293 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 58: /* primary_key: COMMA PRIMARY KEY LBRACE attr_list RBRACE  */
#line 491 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.key_list) = (yyvsp[-1].key_list);
    }
#line 2301 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 59: /* attr_list: ID  */
#line 497 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
       {
      (yyval.key_list) = new vector<string>();
      (yyval.key_list)->push_back((yyvsp[0].cstring));
    }
#line 2310 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 60: /* attr_list: ID COMMA attr_list  */
#line 501 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                         {
      if ((yyvsp[0].key_list) != nullptr) {
        (yyval.key_list) = (yyvsp[0].key_list);
      } else {
        (yyval.key_list) = new vector<string>;
      }

      (yyval.key_list)->insert((yyval.key_list)->begin(), (yyvsp[-2].cstring));
    }
#line 2324 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 61: /* insert_stmt: INSERT INTO ID VALUES LBRACE value_list RBRACE  */
#line 514 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_INSERT);
      (yyval.sql_node)->insertion.relation_name = (yyvsp[-4].cstring);
      (yyval.sql_node)->insertion.values.swap(*(yyvsp[-1].value_list));
      delete (yyvsp[-1].value_list);
    }
#line 2335 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 62: /* value_list: value  */
#line 524 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.value_list) = new vector<Value>;
      (yyval.value_list)->emplace_back(*(yyvsp[0].value));
      delete (yyvsp[0].value);
    }
#line 2345 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 63: /* value_list: value_list COMMA value  */
#line 529 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                             { 
      (yyval.value_list) = (yyvsp[-2].value_list);
      (yyval.value_list)->emplace_back(*(yyvsp[0].value));
      delete (yyvsp[0].value);
    }
#line 2355 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 64: /* value: NUMBER  */
#line 536 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
           {
      (yyval.value) = new Value((int)(yyvsp[0].number));
      (yyloc) = (yylsp[0]);
    }
#line 2364 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 65: /* value: FLOAT  */
#line 540 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
           {
      (yyval.value) = new Value((float)(yyvsp[0].floats));
      (yyloc) = (yylsp[0]);
    }
#line 2373 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 66: /* value: SSS  */
#line 544 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
         {
      char *tmp = common::substr((yyvsp[0].cstring),1,strlen((yyvsp[0].cstring))-2);
      (yyval.value) = new Value(tmp);
      free(tmp);
    }
#line 2383 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 67: /* value: NULL_T  */
#line 549 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            {
      (yyval.value) = new Value();
      (yyval.value)->set_null();
      (yyval.value)->set_type(AttrType::UNDEFINED);  // NULL值类型标识
      (yyloc) = (yylsp[0]);
    }
#line 2394 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 68: /* storage_format: %empty  */
#line 558 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.cstring) = nullptr;
    }
#line 2402 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 69: /* storage_format: STORAGE FORMAT EQ ID  */
#line 562 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.cstring) = (yyvsp[0].cstring);
    }
#line 2410 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 70: /* delete_stmt: DELETE FROM ID where  */
#line 569 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DELETE);
      (yyval.sql_node)->deletion.relation_name = (yyvsp[-1].cstring);
      if ((yyvsp[0].condition_list) != nullptr) {
        (yyval.sql_node)->deletion.conditions.swap(*(yyvsp[0].condition_list));
        delete (yyvsp[0].condition_list);
      }
    }
#line 2423 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 71: /* update_stmt: UPDATE ID SET update_list where  */
#line 580 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_UPDATE);
      (yyval.sql_node)->update.relation_name = (yyvsp[-3].cstring);
      (yyval.sql_node)->update.attribute_names.swap((yyvsp[-1].update_list)->attribute_names);
      (yyval.sql_node)->update.expressions.swap((yyvsp[-1].update_list)->expressions);
      if ((yyvsp[0].condition_list) != nullptr) {
        (yyval.sql_node)->update.conditions.swap(*(yyvsp[0].condition_list));
        delete (yyvsp[0].condition_list);
      }
      delete (yyvsp[-1].update_list);
      // 不需要 free($2)，sql_parse 会统一清理 allocated_strings
    }
#line 2440 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 72: /* update_list: ID EQ expression  */
#line 596 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.update_list) = new UpdateList();
      (yyval.update_list)->attribute_names.push_back((yyvsp[-2].cstring));
      (yyval.update_list)->expressions.push_back((yyvsp[0].expression));
      // 不需要 free($1)，sql_parse 会统一清理 allocated_strings
    }
#line 2451 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 73: /* update_list: update_list COMMA ID EQ expression  */
#line 603 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.update_list) = (yyvsp[-4].update_list);
      (yyval.update_list)->attribute_names.push_back((yyvsp[-2].cstring));
      (yyval.update_list)->expressions.push_back((yyvsp[0].expression));
      // 不需要 free($3)，sql_parse 会统一清理 allocated_strings
    }
#line 2462 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 74: /* select_stmt: SELECT expression_list FROM rel_list where group_by having  */
#line 612 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SELECT);
      if ((yyvsp[-5].expression_list) != nullptr) {
        (yyval.sql_node)->selection.expressions.swap(*(yyvsp[-5].expression_list));
        delete (yyvsp[-5].expression_list);
      }

      if ((yyvsp[-3].relation_list) != nullptr) {
        (yyval.sql_node)->selection.relations.swap(*(yyvsp[-3].relation_list));
        delete (yyvsp[-3].relation_list);
      }

      if ((yyvsp[-2].condition_list) != nullptr) {
        (yyval.sql_node)->selection.conditions.swap(*(yyvsp[-2].condition_list));
        delete (yyvsp[-2].condition_list);
      }

      if ((yyvsp[-1].expression_list) != nullptr) {
        (yyval.sql_node)->selection.group_by.swap(*(yyvsp[-1].expression_list));
        delete (yyvsp[-1].expression_list);
      }

      if ((yyvsp[0].condition_list) != nullptr) {
        (yyval.sql_node)->selection.having.swap(*(yyvsp[0].condition_list));
        delete (yyvsp[0].condition_list);
      }
    }
#line 2494 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 75: /* select_stmt: SELECT expression_list WHERE condition_list  */
#line 640 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SELECT);
      if ((yyvsp[-2].expression_list) != nullptr) {
        (yyval.sql_node)->selection.expressions.swap(*(yyvsp[-2].expression_list));
        delete (yyvsp[-2].expression_list);
      }
      
      if ((yyvsp[0].condition_list) != nullptr) {
        (yyval.sql_node)->selection.conditions.swap(*(yyvsp[0].condition_list));
        delete (yyvsp[0].condition_list);
      }
      // 不设置relations，表示没有FROM子句
    }
#line 2512 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 76: /* select_stmt: SELECT expression_list  */
#line 654 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SELECT);
      if ((yyvsp[0].expression_list) != nullptr) {
        (yyval.sql_node)->selection.expressions.swap(*(yyvsp[0].expression_list));
        delete (yyvsp[0].expression_list);
      }
      // 不设置relations，表示没有FROM子句
    }
#line 2525 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 77: /* calc_stmt: CALC expression_list  */
#line 665 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CALC);
      (yyval.sql_node)->calc.expressions.swap(*(yyvsp[0].expression_list));
      delete (yyvsp[0].expression_list);
    }
#line 2535 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 78: /* expression_list: expression  */
#line 674 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.expression_list) = new vector<unique_ptr<Expression>>;
      (yyval.expression_list)->emplace_back((yyvsp[0].expression));
    }
#line 2544 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 79: /* expression_list: expression COMMA expression_list  */
#line 679 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      if ((yyvsp[0].expression_list) != nullptr) {
        (yyval.expression_list) = (yyvsp[0].expression_list);
      } else {
        (yyval.expression_list) = new vector<unique_ptr<Expression>>;
      }
      (yyval.expression_list)->emplace((yyval.expression_list)->begin(), (yyvsp[-2].expression));
    }
#line 2557 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 80: /* expression: expression '+' expression  */
#line 689 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                              {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::ADD, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2565 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 81: /* expression: expression '-' expression  */
#line 692 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::SUB, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2573 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 82: /* expression: expression '*' expression  */
#line 695 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::MUL, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2581 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 83: /* expression: expression '/' expression  */
#line 698 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                {
      printf("DEBUG: Creating DIV expression\n");
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::DIV, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2590 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 84: /* expression: LBRACE expression RBRACE  */
#line 702 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                               {
      (yyval.expression) = (yyvsp[-1].expression);
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
    }
#line 2599 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 85: /* expression: '-' expression  */
#line 706 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                  {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::NEGATIVE, (yyvsp[0].expression), nullptr, sql_string, &(yyloc));
    }
#line 2607 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 86: /* expression: value  */
#line 709 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            {
      (yyval.expression) = new ValueExpr(*(yyvsp[0].value));
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
      delete (yyvsp[0].value);
    }
#line 2617 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 87: /* expression: rel_attr  */
#line 714 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
               {
      RelAttrSqlNode *node = (yyvsp[0].rel_attr);
      (yyval.expression) = new UnboundFieldExpr(node->relation_name, node->attribute_name);
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
      delete (yyvsp[0].rel_attr);
    }
#line 2628 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 88: /* expression: '*'  */
#line 720 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
          {
      (yyval.expression) = new StarExpr();
    }
#line 2636 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 89: /* expression: COUNT LBRACE expression RBRACE  */
#line 723 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                     {
      (yyval.expression) = create_aggregate_expression("count", (yyvsp[-1].expression), sql_string, &(yyloc));
    }
#line 2644 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 90: /* expression: COUNT LBRACE expression_list RBRACE  */
#line 726 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                          {
      (yyval.expression) = create_aggregate_expression_multi("count", (yyvsp[-1].expression_list), sql_string, &(yyloc));
    }
#line 2652 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 91: /* expression: SUM LBRACE expression RBRACE  */
#line 729 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                   {
      (yyval.expression) = create_aggregate_expression("sum", (yyvsp[-1].expression), sql_string, &(yyloc));
    }
#line 2660 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 92: /* expression: SUM LBRACE expression_list RBRACE  */
#line 732 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                        {
      (yyval.expression) = create_aggregate_expression_multi("sum", (yyvsp[-1].expression_list), sql_string, &(yyloc));
    }
#line 2668 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 93: /* expression: AVG LBRACE expression RBRACE  */
#line 735 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                   {
      (yyval.expression) = create_aggregate_expression("avg", (yyvsp[-1].expression), sql_string, &(yyloc));
    }
#line 2676 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 94: /* expression: AVG LBRACE expression_list RBRACE  */
#line 738 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                        {
      (yyval.expression) = create_aggregate_expression_multi("avg", (yyvsp[-1].expression_list), sql_string, &(yyloc));
    }
#line 2684 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 95: /* expression: MAX LBRACE expression RBRACE  */
#line 741 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                   {
      (yyval.expression) = create_aggregate_expression("max", (yyvsp[-1].expression), sql_string, &(yyloc));
    }
#line 2692 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 96: /* expression: MAX LBRACE expression_list RBRACE  */
#line 744 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                        {
      (yyval.expression) = create_aggregate_expression_multi("max", (yyvsp[-1].expression_list), sql_string, &(yyloc));
    }
#line 2700 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 97: /* expression: MIN LBRACE expression RBRACE  */
#line 747 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                   {
      (yyval.expression) = create_aggregate_expression("min", (yyvsp[-1].expression), sql_string, &(yyloc));
    }
#line 2708 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 98: /* expression: MIN LBRACE expression_list RBRACE  */
#line 750 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                        {
      (yyval.expression) = create_aggregate_expression_multi("min", (yyvsp[-1].expression_list), sql_string, &(yyloc));
    }
#line 2716 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 99: /* rel_attr: ID  */
#line 756 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
       {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->attribute_name = (yyvsp[0].cstring);
    }
#line 2725 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 100: /* rel_attr: ID DOT ID  */
#line 760 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->relation_name  = (yyvsp[-2].cstring);
      (yyval.rel_attr)->attribute_name = (yyvsp[0].cstring);
    }
#line 2735 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 101: /* relation: ID  */
#line 768 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
       {
      (yyval.cstring) = (yyvsp[0].cstring);
    }
#line 2743 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 102: /* rel_list: relation  */
#line 773 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
             {
      (yyval.relation_list) = new vector<string>();
      (yyval.relation_list)->push_back((yyvsp[0].cstring));
    }
#line 2752 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 103: /* rel_list: relation COMMA rel_list  */
#line 777 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                              {
      if ((yyvsp[0].relation_list) != nullptr) {
        (yyval.relation_list) = (yyvsp[0].relation_list);
      } else {
        (yyval.relation_list) = new vector<string>;
      }

      (yyval.relation_list)->insert((yyval.relation_list)->begin(), (yyvsp[-2].cstring));
    }
#line 2766 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 104: /* where: %empty  */
#line 790 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.condition_list) = nullptr;
    }
#line 2774 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 105: /* where: WHERE condition_list  */
#line 793 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                           {
      (yyval.condition_list) = (yyvsp[0].condition_list);  
    }
#line 2782 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 106: /* having: %empty  */
#line 799 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.condition_list) = nullptr;
    }
#line 2790 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 107: /* having: HAVING condition_list  */
#line 802 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                            {
      (yyval.condition_list) = (yyvsp[0].condition_list);
    }
#line 2798 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 108: /* condition_list: %empty  */
#line 808 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.condition_list) = nullptr;
    }
#line 2806 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 109: /* condition_list: condition  */
#line 811 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                {
      (yyval.condition_list) = new vector<ConditionSqlNode>;
      (yyval.condition_list)->push_back(*(yyvsp[0].condition));
      delete (yyvsp[0].condition);
    }
#line 2816 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 110: /* condition_list: condition AND condition_list  */
#line 816 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                   {
      if ((yyvsp[0].condition_list) == nullptr) {
        (yyval.condition_list) = new vector<ConditionSqlNode>;
      } else {
        (yyval.condition_list) = (yyvsp[0].condition_list);
      }
      (yyval.condition_list)->insert((yyval.condition_list)->begin(), *(yyvsp[-2].condition));
      delete (yyvsp[-2].condition);
    }
#line 2830 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 111: /* condition: rel_attr comp_op value  */
#line 828 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      printf("DEBUG: simple condition rel_attr comp_op value -> converting to expression\n");
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->comp = (yyvsp[-1].comp);
      
      // 将rel_attr转换为UnboundFieldExpr
      RelAttrSqlNode *node = (yyvsp[-2].rel_attr);
      (yyval.condition)->left_expression = new UnboundFieldExpr(node->relation_name, node->attribute_name);
      
      // 将value转换为ValueExpr
      (yyval.condition)->right_expression = new ValueExpr(*(yyvsp[0].value));
      
      (yyval.condition)->is_expression_condition = true;
      (yyval.condition)->left_is_attr = 0;
      (yyval.condition)->right_is_attr = 0;
      
      delete (yyvsp[-2].rel_attr);
      delete (yyvsp[0].value);
    }
#line 2854 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 112: /* condition: expression comp_op expression  */
#line 848 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      printf("DEBUG: unified condition expression comp_op expression\n");
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->comp = (yyvsp[-1].comp);
      (yyval.condition)->left_expression = (yyvsp[-2].expression);
      (yyval.condition)->right_expression = (yyvsp[0].expression);
      (yyval.condition)->is_expression_condition = true;
      
      // 清零旧字段以确保一致性
      (yyval.condition)->left_is_attr = 0;
      (yyval.condition)->right_is_attr = 0;
    }
#line 2871 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 113: /* condition: expression IS NULL_T  */
#line 861 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      printf("DEBUG: IS NULL condition\n");
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->comp = IS_NULL;
      (yyval.condition)->left_expression = (yyvsp[-2].expression);
      (yyval.condition)->right_expression = nullptr;
      (yyval.condition)->is_expression_condition = true;
      
      // 清零旧字段以确保一致性
      (yyval.condition)->left_is_attr = 0;
      (yyval.condition)->right_is_attr = 0;
    }
#line 2888 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 114: /* condition: expression IS NOT NULL_T  */
#line 874 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      printf("DEBUG: IS NOT NULL condition\n");
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->comp = IS_NOT_NULL;
      (yyval.condition)->left_expression = (yyvsp[-3].expression);
      (yyval.condition)->right_expression = nullptr;
      (yyval.condition)->is_expression_condition = true;
      
      // 清零旧字段以确保一致性
      (yyval.condition)->left_is_attr = 0;
      (yyval.condition)->right_is_attr = 0;
    }
#line 2905 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 115: /* condition: expression  */
#line 887 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      printf("DEBUG: single expression condition\n");
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->comp = NO_OP;  // 标识单独表达式条件
      (yyval.condition)->left_expression = (yyvsp[0].expression);
      (yyval.condition)->right_expression = nullptr;
      (yyval.condition)->is_expression_condition = true;
      
      // 清零旧字段以确保一致性
      (yyval.condition)->left_is_attr = 0;
      (yyval.condition)->right_is_attr = 0;
    }
#line 2922 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 116: /* condition: rel_attr IN LBRACE value_list RBRACE  */
#line 900 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 1;
      (yyval.condition)->left_attr = *(yyvsp[-4].rel_attr);
      (yyval.condition)->right_is_attr = 0;
      (yyval.condition)->comp = IN_OP;
      (yyval.condition)->right_values = *(yyvsp[-1].value_list);

      delete (yyvsp[-4].rel_attr);
      delete (yyvsp[-1].value_list);
    }
#line 2938 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 117: /* condition: rel_attr NOT IN LBRACE value_list RBRACE  */
#line 912 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 1;
      (yyval.condition)->left_attr = *(yyvsp[-5].rel_attr);
      (yyval.condition)->right_is_attr = 0;
      (yyval.condition)->comp = NOT_IN_OP;
      (yyval.condition)->right_values = *(yyvsp[-1].value_list);

      delete (yyvsp[-5].rel_attr);
      delete (yyvsp[-1].value_list);
    }
#line 2954 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 118: /* condition: value IN LBRACE value_list RBRACE  */
#line 924 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 0;
      (yyval.condition)->left_value = *(yyvsp[-4].value);
      (yyval.condition)->right_is_attr = 0;
      (yyval.condition)->comp = IN_OP;
      (yyval.condition)->right_values = *(yyvsp[-1].value_list);

      delete (yyvsp[-4].value);
      delete (yyvsp[-1].value_list);
    }
#line 2970 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 119: /* condition: value NOT IN LBRACE value_list RBRACE  */
#line 936 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 0;
      (yyval.condition)->left_value = *(yyvsp[-5].value);
      (yyval.condition)->right_is_attr = 0;
      (yyval.condition)->comp = NOT_IN_OP;
      (yyval.condition)->right_values = *(yyvsp[-1].value_list);

      delete (yyvsp[-5].value);
      delete (yyvsp[-1].value_list);
    }
#line 2986 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 120: /* condition: rel_attr IN LBRACE select_stmt RBRACE  */
#line 948 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 1;
      (yyval.condition)->left_attr = *(yyvsp[-4].rel_attr);
      (yyval.condition)->right_is_attr = 0;
      (yyval.condition)->comp = IN_OP;
      (yyval.condition)->has_subquery = true;
      (yyval.condition)->subquery = SelectSqlNode::create_copy(&((yyvsp[-1].sql_node)->selection)).release();

      delete (yyvsp[-4].rel_attr);
      delete (yyvsp[-1].sql_node);
    }
#line 3003 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 121: /* condition: rel_attr NOT IN LBRACE select_stmt RBRACE  */
#line 961 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 1;
      (yyval.condition)->left_attr = *(yyvsp[-5].rel_attr);
      (yyval.condition)->right_is_attr = 0;
      (yyval.condition)->comp = NOT_IN_OP;
      (yyval.condition)->has_subquery = true;
      (yyval.condition)->subquery = SelectSqlNode::create_copy(&((yyvsp[-1].sql_node)->selection)).release();

      delete (yyvsp[-5].rel_attr);
      delete (yyvsp[-1].sql_node);
    }
#line 3020 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 122: /* condition: EXISTS LBRACE select_stmt RBRACE  */
#line 974 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 0;
      (yyval.condition)->right_is_attr = 0;
      (yyval.condition)->comp = EXISTS_OP;
      (yyval.condition)->has_subquery = true;
      (yyval.condition)->subquery = SelectSqlNode::create_copy(&((yyvsp[-1].sql_node)->selection)).release();

      delete (yyvsp[-1].sql_node);
    }
#line 3035 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 123: /* condition: NOT EXISTS LBRACE select_stmt RBRACE  */
#line 985 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 0;
      (yyval.condition)->right_is_attr = 0;
      (yyval.condition)->comp = NOT_EXISTS_OP;
      (yyval.condition)->has_subquery = true;
      (yyval.condition)->subquery = SelectSqlNode::create_copy(&((yyvsp[-1].sql_node)->selection)).release();

      delete (yyvsp[-1].sql_node);
    }
#line 3050 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 124: /* condition: LBRACE select_stmt RBRACE comp_op rel_attr  */
#line 996 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 0;
      (yyval.condition)->right_is_attr = 1;
      (yyval.condition)->right_attr = *(yyvsp[0].rel_attr);
      (yyval.condition)->comp = (yyvsp[-1].comp);
      (yyval.condition)->has_subquery = true;
      (yyval.condition)->subquery = SelectSqlNode::create_copy(&((yyvsp[-3].sql_node)->selection)).release();

      delete (yyvsp[-3].sql_node);
      delete (yyvsp[0].rel_attr);
    }
#line 3067 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 125: /* condition: rel_attr comp_op LBRACE select_stmt RBRACE  */
#line 1009 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 1;
      (yyval.condition)->left_attr = *(yyvsp[-4].rel_attr);
      (yyval.condition)->right_is_attr = 0;
      (yyval.condition)->comp = (yyvsp[-3].comp);
      (yyval.condition)->has_subquery = true;
      (yyval.condition)->subquery = SelectSqlNode::create_copy(&((yyvsp[-1].sql_node)->selection)).release();

      delete (yyvsp[-4].rel_attr);
      delete (yyvsp[-1].sql_node);
    }
#line 3084 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 126: /* comp_op: EQ  */
#line 1024 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
         { (yyval.comp) = EQUAL_TO; }
#line 3090 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 127: /* comp_op: LT  */
#line 1025 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
         { (yyval.comp) = LESS_THAN; }
#line 3096 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 128: /* comp_op: GT  */
#line 1026 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
         { (yyval.comp) = GREAT_THAN; }
#line 3102 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 129: /* comp_op: LE  */
#line 1027 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
         { (yyval.comp) = LESS_EQUAL; }
#line 3108 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 130: /* comp_op: GE  */
#line 1028 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
         { (yyval.comp) = GREAT_EQUAL; }
#line 3114 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 131: /* comp_op: NE  */
#line 1029 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
         { (yyval.comp) = NOT_EQUAL; }
#line 3120 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 132: /* comp_op: LIKE  */
#line 1030 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
           { (yyval.comp) = LIKE_OP; }
#line 3126 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 133: /* group_by: %empty  */
#line 1037 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.expression_list) = nullptr;
    }
#line 3134 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 134: /* group_by: GROUP BY expression_list  */
#line 1041 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.expression_list) = (yyvsp[0].expression_list); 
    }
#line 3142 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 135: /* load_data_stmt: LOAD DATA INFILE SSS INTO TABLE ID  */
#line 1047 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      char *tmp_file_name = common::substr((yyvsp[-3].cstring), 1, strlen((yyvsp[-3].cstring)) - 2);
      
      (yyval.sql_node) = new ParsedSqlNode(SCF_LOAD_DATA);
      (yyval.sql_node)->load_data.relation_name = (yyvsp[0].cstring);
      (yyval.sql_node)->load_data.file_name = tmp_file_name;
      free(tmp_file_name);
    }
#line 3155 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 136: /* explain_stmt: EXPLAIN command_wrapper  */
#line 1059 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_EXPLAIN);
      (yyval.sql_node)->explain.sql_node = unique_ptr<ParsedSqlNode>((yyvsp[0].sql_node));
    }
#line 3164 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 137: /* set_variable_stmt: SET ID EQ value  */
#line 1067 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SET_VARIABLE);
      (yyval.sql_node)->set_variable.name  = (yyvsp[-2].cstring);
      (yyval.sql_node)->set_variable.value = *(yyvsp[0].value);
      delete (yyvsp[0].value);
    }
#line 3175 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;


#line 3179 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken, &yylloc};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (&yylloc, sql_string, sql_result, scanner, yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc, sql_string, sql_result, scanner);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp, sql_string, sql_result, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, sql_string, sql_result, scanner, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, sql_string, sql_result, scanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp, sql_string, sql_result, scanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 1078 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"

//_____________________________________________________________________
extern void scan_string(const char *str, yyscan_t scanner);


//整个数据库系统的编译器前端
//const char *str：输入参数，要解析的SQL字符串
//ParsedSqlResult *sql_result：输出参数，存储解析结果的容器




int sql_parse(const char *s, ParsedSqlResult *sql_result) {  
  
  //yyscan_t：这是一个类型别名，实际上是词法分析器的句柄
  //scanner：词法分析器的实例，用于将SQL字符串分解成tokens
  yyscan_t scanner;


  //allocated_strings是一个内存管理的容器
  std::vector<char *> allocated_strings;   

  //初始化词法分析器，并传递额外的自定义数据
  yylex_init_extra(static_cast<void*>(&allocated_strings),&scanner);
  scan_string(s, scanner);
  int result = yyparse(s, sql_result, scanner);

  for (char *ptr : allocated_strings) {
    free(ptr);
  }
  allocated_strings.clear();

  yylex_destroy(scanner);
  return result;
}

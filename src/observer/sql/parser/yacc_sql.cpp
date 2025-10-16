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
  YYSYMBOL_on_conditions = 125,            /* on_conditions  */
  YYSYMBOL_join_list = 126,                /* join_list  */
  YYSYMBOL_group_by = 127,                 /* group_by  */
  YYSYMBOL_load_data_stmt = 128,           /* load_data_stmt  */
  YYSYMBOL_explain_stmt = 129,             /* explain_stmt  */
  YYSYMBOL_set_variable_stmt = 130,        /* set_variable_stmt  */
  YYSYMBOL_opt_semicolon = 131             /* opt_semicolon  */
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
#define YYFINAL  79
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   341

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  81
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  51
/* YYNRULES -- Number of rules.  */
#define YYNRULES  136
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  275

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
       0,   259,   259,   267,   268,   269,   270,   271,   272,   273,
     274,   275,   276,   277,   278,   279,   280,   281,   282,   283,
     284,   285,   286,   287,   288,   292,   298,   303,   309,   315,
     321,   327,   333,   340,   346,   353,   363,   375,   380,   387,
     395,   402,   423,   429,   438,   446,   454,   462,   473,   474,
     478,   481,   482,   483,   484,   485,   489,   492,   499,   503,
     515,   525,   531,   538,   542,   546,   551,   560,   563,   570,
     581,   597,   604,   614,   647,   661,   672,   681,   686,   697,
     700,   703,   706,   710,   714,   717,   722,   728,   731,   734,
     737,   740,   743,   746,   749,   755,   765,   775,   782,   789,
     796,   806,   810,   818,   823,   827,   840,   843,   849,   852,
     858,   861,   866,   877,   890,   903,   916,   932,   933,   934,
     935,   936,   937,   938,   939,   944,   956,   976,   979,   994,
    1018,  1021,  1027,  1039,  1047,  1056,  1057
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
  "condition_list", "condition", "comp_op", "on_conditions", "join_list",
  "group_by", "load_data_stmt", "explain_stmt", "set_variable_stmt",
  "opt_semicolon", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-250)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     234,    24,    58,    96,    96,   -55,     8,  -250,    10,   -16,
     -11,  -250,  -250,  -250,  -250,  -250,    -8,    25,   234,    28,
      65,    72,  -250,  -250,  -250,  -250,  -250,  -250,  -250,  -250,
    -250,  -250,  -250,  -250,  -250,  -250,  -250,  -250,  -250,  -250,
    -250,  -250,  -250,  -250,    18,    27,    94,    45,    48,    43,
    -250,    30,    97,    98,    99,   100,   102,   103,  -250,  -250,
      78,  -250,    96,  -250,  -250,  -250,   232,  -250,    37,  -250,
    -250,    86,    66,    69,    88,    64,    90,  -250,    74,  -250,
    -250,  -250,   120,   104,    76,  -250,   111,   116,    -9,   122,
     113,    96,    96,    96,    96,   132,    81,   -29,    96,   105,
     135,    96,    96,    96,    96,    93,    96,   106,   134,   149,
     126,     1,   127,  -250,   131,   137,   156,   138,  -250,  -250,
     132,   171,    17,   117,   159,   167,   174,   179,  -250,  -250,
     184,    43,   -25,   -25,   -29,   -29,  -250,   185,   145,   239,
    -250,   168,  -250,   194,    96,  -250,   147,   -14,  -250,   182,
     -13,   198,  -250,   196,   158,  -250,   200,  -250,  -250,  -250,
    -250,  -250,  -250,  -250,    43,   203,   209,    93,   169,   -36,
      20,    50,  -250,  -250,  -250,  -250,  -250,  -250,  -250,    96,
      96,     1,  -250,    96,   180,  -250,   225,  -250,  -250,  -250,
    -250,  -250,    -1,   -41,   235,   195,   241,  -250,   243,   245,
    -250,  -250,  -250,    93,   205,   256,  -250,  -250,   242,   143,
    -250,    67,  -250,   143,   202,   210,   211,   247,  -250,   228,
    -250,   236,  -250,    73,   195,  -250,  -250,   237,    93,   282,
     278,  -250,  -250,     1,    96,  -250,  -250,   266,  -250,   268,
     246,  -250,  -250,   231,    80,    96,   260,    96,    96,  -250,
    -250,   143,   273,   240,   248,  -250,  -250,   258,  -250,    96,
    -250,  -250,  -250,   296,   299,   257,    96,  -250,   240,  -250,
    -250,   224,  -250,    96,  -250
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,    27,     0,     0,
       0,    28,    29,    30,    26,    25,     0,     0,     0,     0,
       0,   135,    24,    23,    16,    17,    18,    19,     9,    10,
      11,    13,    14,    15,    12,     8,     5,     7,     6,     4,
       3,    20,    21,    22,     0,     0,     0,     0,     0,     0,
      66,     0,     0,     0,     0,     0,     0,     0,    63,    64,
     101,    65,     0,    87,    85,    76,    77,    86,    75,    34,
      33,     0,     0,     0,     0,     0,     0,   133,     0,     1,
     136,     2,     0,     0,     0,    31,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    84,     0,     0,
       0,     0,     0,     0,     0,     0,   110,     0,     0,   106,
       0,     0,     0,    32,     0,     0,     0,     0,    94,    83,
       0,    87,     0,     0,     0,     0,     0,     0,   102,    78,
       0,     0,    79,    80,    81,    82,   103,   104,   127,   116,
      74,   111,    40,     0,   110,    69,     0,   106,   134,     0,
       0,    56,    42,     0,     0,    39,     0,    88,    89,    90,
      91,    92,    93,    99,     0,     0,     0,     0,     0,   106,
       0,     0,   123,   117,   122,   118,   120,   119,   121,     0,
     110,     0,   107,     0,     0,    70,     0,    51,    52,    53,
      55,    54,    47,     0,     0,     0,     0,   100,     0,     0,
      97,    95,   105,     0,     0,   130,   124,   114,     0,   113,
     112,     0,    61,    71,     0,     0,     0,     0,    45,     0,
      43,    67,    37,     0,     0,    98,    96,     0,     0,     0,
     108,   115,    60,     0,     0,   132,    50,     0,    48,     0,
       0,    41,    35,     0,     0,     0,     0,     0,   110,    73,
      62,    72,    46,     0,     0,    38,    36,     0,   128,     0,
     131,   109,    44,    58,     0,     0,     0,   129,     0,    57,
      68,   125,    59,     0,   126
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -250,  -250,   306,  -250,  -250,  -250,  -250,  -250,  -250,  -250,
    -250,  -250,  -250,  -250,   114,  -250,  -250,  -250,  -250,   133,
      75,  -250,  -250,  -250,    71,  -250,  -250,  -109,  -250,  -250,
    -250,  -250,   -46,  -250,    -4,   -48,  -250,  -197,   173,  -139,
    -250,  -140,  -250,    84,  -249,  -250,  -250,  -250,  -250,  -250,
    -250
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,   223,    33,    34,    35,   151,   152,
     218,   237,   192,   194,   264,    36,   211,    64,   241,    37,
      38,   147,    39,    40,    65,    66,    67,   137,   138,   145,
     249,   140,   141,   179,   258,   169,   230,    41,    42,    43,
      81
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      68,    88,   148,    87,   182,    99,   227,   144,   185,    99,
     267,   184,   219,    69,    97,   119,   187,   188,   189,   190,
      70,    71,   216,   191,   274,    99,    73,   150,   204,   144,
     205,   246,   100,   217,    50,    44,   100,    45,    46,    78,
     210,   158,   122,   123,   124,   125,   126,   103,   104,   127,
      72,    99,   100,   132,   133,   134,   135,    74,   139,     4,
      75,   101,   102,   103,   104,    79,    49,    58,    59,    47,
      61,    48,   212,    76,   156,    80,    50,    51,   100,   105,
     106,   130,   206,   207,   208,   165,    82,   101,   102,   103,
     104,   232,   233,    89,   129,    83,   139,   242,   243,    52,
      53,    54,    55,    56,   256,   243,    57,    84,   261,    58,
      59,    60,    61,    85,    62,    63,    86,    96,   198,    49,
      90,    91,    92,    93,   250,    94,    95,   166,   107,    50,
      51,   209,   139,   110,   108,   213,    49,   109,   111,   112,
     118,   159,   113,   114,   116,   120,    50,    51,     4,   128,
     115,    99,    52,    53,    54,    55,    56,   117,   131,    57,
     199,   136,    58,    59,    60,    61,   130,    62,    63,    52,
      53,    54,    55,    56,   142,   143,    57,    99,   100,    58,
      59,    60,    61,   160,    62,   121,   251,   101,   102,   103,
     104,   161,   144,    99,   146,   157,   149,   257,   162,   150,
     139,    99,   154,   163,   100,   153,   155,   164,    99,   168,
     167,   257,   180,   101,   102,   103,   104,   181,   271,   195,
     100,   183,   186,   193,   197,   257,   196,   200,   100,   101,
     102,   103,   104,   201,   203,   100,   215,   101,   102,   103,
     104,     1,     2,   260,   101,   102,   103,   104,   214,     3,
       4,     5,     6,     7,     8,     9,    10,    98,    99,   221,
      11,    12,    13,   222,   224,   229,    99,   225,   273,   226,
     228,    14,    15,   170,   171,   231,   234,   236,   235,    16,
     238,    17,   239,   245,    18,   100,   247,   240,   248,    19,
     252,   253,   170,   100,   101,   102,   103,   104,   254,   255,
     100,   172,   101,   102,   103,   104,   259,   217,   263,   101,
     102,   103,   104,   173,   174,   175,   176,   177,   178,   100,
     172,   268,   265,   269,    77,   270,   220,   262,   101,   102,
     103,   104,   173,   174,   175,   176,   177,   178,   244,   272,
     202,   266
};

static const yytype_int16 yycheck[] =
{
       4,    49,   111,    49,   144,    34,   203,    43,   147,    34,
     259,    25,    53,    68,    62,    24,    29,    30,    31,    32,
      12,    13,    23,    36,   273,    34,    42,    68,    64,    43,
     169,   228,    61,    34,    33,    11,    61,    13,    14,    11,
     180,    24,    90,    91,    92,    93,    94,    72,    73,    95,
      40,    34,    61,   101,   102,   103,   104,    68,   106,    16,
      68,    70,    71,    72,    73,     0,    23,    66,    67,    11,
      69,    13,   181,    48,   120,     3,    33,    34,    61,    42,
      43,    61,    62,    33,    34,   131,    68,    70,    71,    72,
      73,    24,    25,    63,    98,    68,   144,    24,    25,    56,
      57,    58,    59,    60,    24,    25,    63,    13,   248,    66,
      67,    68,    69,    68,    71,    72,    68,    39,   164,    23,
      23,    23,    23,    23,   233,    23,    23,   131,    42,    33,
      34,   179,   180,    45,    68,   183,    23,    68,    74,    49,
      24,    24,    68,    23,    68,    23,    33,    34,    16,    68,
      46,    34,    56,    57,    58,    59,    60,    46,    23,    63,
     164,    68,    66,    67,    68,    69,    61,    71,    72,    56,
      57,    58,    59,    60,    68,    41,    63,    34,    61,    66,
      67,    68,    69,    24,    71,    72,   234,    70,    71,    72,
      73,    24,    43,    34,    68,    24,    69,   245,    24,    68,
     248,    34,    46,    24,    61,    68,    68,    23,    34,    64,
      25,   259,    44,    70,    71,    72,    73,    23,   266,    23,
      61,    74,    40,    25,    24,   273,    68,    24,    61,    70,
      71,    72,    73,    24,    65,    61,    11,    70,    71,    72,
      73,     7,     8,   247,    70,    71,    72,    73,    68,    15,
      16,    17,    18,    19,    20,    21,    22,    25,    34,    24,
      26,    27,    28,    68,    23,     9,    34,    24,    44,    24,
      65,    37,    38,    34,    35,    33,    74,    66,    68,    45,
      33,    47,    54,    46,    50,    61,     4,    51,    10,    55,
      24,    23,    34,    61,    70,    71,    72,    73,    52,    68,
      61,    62,    70,    71,    72,    73,    46,    34,    68,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    61,
      62,    25,    74,    24,    18,    68,   193,   252,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,   224,   268,
     167,   257
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     7,     8,    15,    16,    17,    18,    19,    20,    21,
      22,    26,    27,    28,    37,    38,    45,    47,    50,    55,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    96,    97,    98,   106,   110,   111,   113,
     114,   128,   129,   130,    11,    13,    14,    11,    13,    23,
      33,    34,    56,    57,    58,    59,    60,    63,    66,    67,
      68,    69,    71,    72,   108,   115,   116,   117,   115,    68,
      12,    13,    40,    42,    68,    68,    48,    83,    11,     0,
       3,   131,    68,    68,    13,    68,    68,   113,   116,    63,
      23,    23,    23,    23,    23,    23,    39,   116,    25,    34,
      61,    70,    71,    72,    73,    42,    43,    42,    68,    68,
      45,    74,    49,    68,    23,    46,    68,    46,    24,    24,
      23,    72,   116,   116,   116,   116,   116,   113,    68,   115,
      61,    23,   116,   116,   116,   116,    68,   118,   119,   116,
     122,   123,    68,    41,    43,   120,    68,   112,   108,    69,
      68,    99,   100,    68,    46,    68,   113,    24,    24,    24,
      24,    24,    24,    24,    23,   113,   115,    25,    64,   126,
      34,    35,    62,    74,    75,    76,    77,    78,    79,   124,
      44,    23,   122,    74,    25,   120,    40,    29,    30,    31,
      32,    36,   103,    25,   104,    23,    68,    24,   113,   115,
      24,    24,   119,    65,    64,   120,    62,    33,    34,   116,
     122,   107,   108,   116,    68,    11,    23,    34,   101,    53,
     100,    24,    68,    95,    23,    24,    24,   118,    65,     9,
     127,    33,    24,    25,    74,    68,    66,   102,    33,    54,
      51,   109,    24,    25,    95,    46,   118,     4,    10,   121,
     108,   116,    24,    23,    52,    68,    24,   116,   125,    46,
     115,   122,   101,    68,   105,    74,   124,   125,    25,    24,
      68,   116,   105,    44,   125
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    81,    82,    83,    83,    83,    83,    83,    83,    83,
      83,    83,    83,    83,    83,    83,    83,    83,    83,    83,
      83,    83,    83,    83,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    94,    95,    95,    96,
      97,    98,    99,    99,   100,   100,   100,   100,   101,   101,
     102,   103,   103,   103,   103,   103,   104,   104,   105,   105,
     106,   107,   107,   108,   108,   108,   108,   109,   109,   110,
     111,   112,   112,   113,   113,   113,   114,   115,   115,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   117,   117,   118,   119,   119,   120,   120,   121,   121,
     122,   122,   122,   123,   123,   123,   123,   124,   124,   124,
     124,   124,   124,   124,   124,   125,   125,   126,   126,   126,
     127,   127,   128,   129,   130,   131,   131
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     2,     2,     8,     9,     1,     3,     5,
       4,     8,     1,     3,     6,     3,     5,     2,     2,     0,
       1,     1,     1,     1,     1,     1,     0,     6,     1,     3,
       7,     1,     3,     1,     1,     1,     1,     0,     4,     4,
       5,     3,     5,     8,     4,     2,     2,     1,     3,     3,
       3,     3,     3,     3,     2,     1,     1,     1,     4,     4,
       4,     4,     4,     4,     3,     5,     6,     5,     6,     4,
       5,     1,     3,     1,     1,     3,     0,     2,     0,     2,
       0,     1,     3,     3,     3,     4,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     3,     5,     0,     5,     6,
       0,     3,     7,     2,     4,     0,     1
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
#line 192 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).key_list); }
#line 1587 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_attr_def_list: /* attr_def_list  */
#line 184 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).attr_infos); }
#line 1593 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_attr_def: /* attr_def  */
#line 185 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).attr_info); }
#line 1599 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_primary_key: /* primary_key  */
#line 192 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).key_list); }
#line 1605 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_attr_list: /* attr_list  */
#line 192 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).key_list); }
#line 1611 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_value_list: /* value_list  */
#line 188 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).value_list); }
#line 1617 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_value: /* value  */
#line 182 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).value); }
#line 1623 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_update_list: /* update_list  */
#line 193 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).update_list); }
#line 1629 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_expression_list: /* expression_list  */
#line 187 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).expression_list); }
#line 1635 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_expression: /* expression  */
#line 186 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).expression); }
#line 1641 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_rel_attr: /* rel_attr  */
#line 183 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).rel_attr); }
#line 1647 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_rel_list: /* rel_list  */
#line 191 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).relation_list); }
#line 1653 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_where: /* where  */
#line 189 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).condition_list); }
#line 1659 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_having: /* having  */
#line 189 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).condition_list); }
#line 1665 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_condition_list: /* condition_list  */
#line 189 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).condition_list); }
#line 1671 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_condition: /* condition  */
#line 181 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).condition); }
#line 1677 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_on_conditions: /* on_conditions  */
#line 189 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).condition_list); }
#line 1683 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_group_by: /* group_by  */
#line 187 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).expression_list); }
#line 1689 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
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
#line 260 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
  {
    unique_ptr<ParsedSqlNode> sql_node = unique_ptr<ParsedSqlNode>((yyvsp[-1].sql_node));
    sql_result->add_sql_node(std::move(sql_node));
  }
#line 1998 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 25: /* exit_stmt: EXIT  */
#line 292 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
         {
      (void)yynerrs;  // 这么写为了消除yynerrs未使用的告警。如果你有更好的方法欢迎提PR
      (yyval.sql_node) = new ParsedSqlNode(SCF_EXIT);
    }
#line 2007 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 26: /* help_stmt: HELP  */
#line 298 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
         {
      (yyval.sql_node) = new ParsedSqlNode(SCF_HELP);
    }
#line 2015 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 27: /* sync_stmt: SYNC  */
#line 303 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
         {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SYNC);
    }
#line 2023 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 28: /* begin_stmt: TRX_BEGIN  */
#line 309 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
               {
      (yyval.sql_node) = new ParsedSqlNode(SCF_BEGIN);
    }
#line 2031 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 29: /* commit_stmt: TRX_COMMIT  */
#line 315 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
               {
      (yyval.sql_node) = new ParsedSqlNode(SCF_COMMIT);
    }
#line 2039 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 30: /* rollback_stmt: TRX_ROLLBACK  */
#line 321 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                  {
      (yyval.sql_node) = new ParsedSqlNode(SCF_ROLLBACK);
    }
#line 2047 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 31: /* drop_table_stmt: DROP TABLE ID  */
#line 327 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                  {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DROP_TABLE);
      (yyval.sql_node)->drop_table.relation_name = (yyvsp[0].cstring);
    }
#line 2056 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 32: /* analyze_table_stmt: ANALYZE TABLE ID  */
#line 333 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                     {
      (yyval.sql_node) = new ParsedSqlNode(SCF_ANALYZE_TABLE);
      (yyval.sql_node)->analyze_table.relation_name = (yyvsp[0].cstring);
    }
#line 2065 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 33: /* show_tables_stmt: SHOW TABLES  */
#line 340 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SHOW_TABLES);
    }
#line 2073 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 34: /* desc_table_stmt: DESC ID  */
#line 346 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
             {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DESC_TABLE);
      (yyval.sql_node)->desc_table.relation_name = (yyvsp[0].cstring);
    }
#line 2082 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 35: /* create_index_stmt: CREATE INDEX ID ON ID LBRACE attribute_name_list RBRACE  */
#line 354 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_INDEX);
      CreateIndexSqlNode &create_index = (yyval.sql_node)->create_index;
      create_index.index_name = (yyvsp[-5].cstring);
      create_index.relation_name = (yyvsp[-3].cstring);
      create_index.is_unique = false;
      create_index.attribute_names = std::move (*(yyvsp[-1].key_list));
      delete (yyvsp[-1].key_list);
    }
#line 2096 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 36: /* create_index_stmt: CREATE UNIQUE INDEX ID ON ID LBRACE attribute_name_list RBRACE  */
#line 364 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_INDEX);
      CreateIndexSqlNode &create_index = (yyval.sql_node)->create_index;
      create_index.index_name = (yyvsp[-5].cstring);
      create_index.relation_name = (yyvsp[-3].cstring);
      create_index.is_unique = true;
      create_index.attribute_names = std::move (*(yyvsp[-1].key_list));
      delete (yyvsp[-1].key_list);
    }
#line 2110 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 37: /* attribute_name_list: ID  */
#line 376 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.key_list) = new vector<string> ();
      (yyval.key_list)->push_back((yyvsp[0].cstring));
    }
#line 2119 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 38: /* attribute_name_list: attribute_name_list COMMA ID  */
#line 381 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.key_list) = (yyvsp[-2].key_list);
      (yyval.key_list)->push_back((yyvsp[0].cstring));
    }
#line 2128 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 39: /* drop_index_stmt: DROP INDEX ID ON ID  */
#line 388 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DROP_INDEX);
      (yyval.sql_node)->drop_index.index_name = (yyvsp[-2].cstring);
      (yyval.sql_node)->drop_index.relation_name = (yyvsp[0].cstring);
    }
#line 2138 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 40: /* show_index_stmt: SHOW INDEX FROM ID  */
#line 396 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SHOW_INDEX);
      (yyval.sql_node)->show_index.relation_name = (yyvsp[0].cstring);  
    }
#line 2147 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 41: /* create_table_stmt: CREATE TABLE ID LBRACE attr_def_list primary_key RBRACE storage_format  */
#line 403 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
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
#line 2169 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 42: /* attr_def_list: attr_def  */
#line 424 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.attr_infos) = new vector<AttrInfoSqlNode>;
      (yyval.attr_infos)->emplace_back(*(yyvsp[0].attr_info));
      delete (yyvsp[0].attr_info);
    }
#line 2179 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 43: /* attr_def_list: attr_def_list COMMA attr_def  */
#line 430 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.attr_infos) = (yyvsp[-2].attr_infos);
      (yyval.attr_infos)->emplace_back(*(yyvsp[0].attr_info));
      delete (yyvsp[0].attr_info);
    }
#line 2189 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 44: /* attr_def: ID type LBRACE number RBRACE nullable_spec  */
#line 439 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-4].number);
      (yyval.attr_info)->name = (yyvsp[-5].cstring);
      (yyval.attr_info)->length = (yyvsp[-2].number);
      (yyval.attr_info)->nullable = ((yyvsp[0].number) == 1);
    }
#line 2201 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 45: /* attr_def: ID type nullable_spec  */
#line 447 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-1].number);
      (yyval.attr_info)->name = (yyvsp[-2].cstring);
      (yyval.attr_info)->length = 4;
      (yyval.attr_info)->nullable = ((yyvsp[0].number) == 1);
    }
#line 2213 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 46: /* attr_def: ID type LBRACE number RBRACE  */
#line 455 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-3].number);
      (yyval.attr_info)->name = (yyvsp[-4].cstring);
      (yyval.attr_info)->length = (yyvsp[-1].number);
      (yyval.attr_info)->nullable = true;  
    }
#line 2225 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 47: /* attr_def: ID type  */
#line 463 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[0].number);
      (yyval.attr_info)->name = (yyvsp[-1].cstring);
      (yyval.attr_info)->length = 4;
      (yyval.attr_info)->nullable = true;  
    }
#line 2237 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 48: /* nullable_spec: NOT NULL_T  */
#line 473 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                        { (yyval.number) = 0; }
#line 2243 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 49: /* nullable_spec: %empty  */
#line 474 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                        { (yyval.number) = 1; }
#line 2249 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 50: /* number: NUMBER  */
#line 478 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
           {(yyval.number) = (yyvsp[0].number);}
#line 2255 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 51: /* type: INT_T  */
#line 481 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
               { (yyval.number) = static_cast<int>(AttrType::INTS); }
#line 2261 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 52: /* type: STRING_T  */
#line 482 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
               { (yyval.number) = static_cast<int>(AttrType::CHARS); }
#line 2267 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 53: /* type: FLOAT_T  */
#line 483 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
               { (yyval.number) = static_cast<int>(AttrType::FLOATS); }
#line 2273 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 54: /* type: VECTOR_T  */
#line 484 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
               { (yyval.number) = static_cast<int>(AttrType::VECTORS); }
#line 2279 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 55: /* type: DATE_T  */
#line 485 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
             { (yyval.number) = static_cast<int>(AttrType::DATES); }
#line 2285 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 56: /* primary_key: %empty  */
#line 489 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.key_list) = nullptr;
    }
#line 2293 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 57: /* primary_key: COMMA PRIMARY KEY LBRACE attr_list RBRACE  */
#line 493 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.key_list) = (yyvsp[-1].key_list);
    }
#line 2301 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 58: /* attr_list: ID  */
#line 499 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
       {
      (yyval.key_list) = new vector<string>();
      (yyval.key_list)->push_back((yyvsp[0].cstring));
    }
#line 2310 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 59: /* attr_list: ID COMMA attr_list  */
#line 503 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
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

  case 60: /* insert_stmt: INSERT INTO ID VALUES LBRACE value_list RBRACE  */
#line 516 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_INSERT);
      (yyval.sql_node)->insertion.relation_name = (yyvsp[-4].cstring);
      (yyval.sql_node)->insertion.values.swap(*(yyvsp[-1].value_list));
      delete (yyvsp[-1].value_list);
    }
#line 2335 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 61: /* value_list: value  */
#line 526 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.value_list) = new vector<Value>;
      (yyval.value_list)->emplace_back(*(yyvsp[0].value));
      delete (yyvsp[0].value);
    }
#line 2345 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 62: /* value_list: value_list COMMA value  */
#line 531 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                             { 
      (yyval.value_list) = (yyvsp[-2].value_list);
      (yyval.value_list)->emplace_back(*(yyvsp[0].value));
      delete (yyvsp[0].value);
    }
#line 2355 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 63: /* value: NUMBER  */
#line 538 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
           {
      (yyval.value) = new Value((int)(yyvsp[0].number));
      (yyloc) = (yylsp[0]);
    }
#line 2364 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 64: /* value: FLOAT  */
#line 542 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
           {
      (yyval.value) = new Value((float)(yyvsp[0].floats));
      (yyloc) = (yylsp[0]);
    }
#line 2373 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 65: /* value: SSS  */
#line 546 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
         {
      char *tmp = common::substr((yyvsp[0].cstring),1,strlen((yyvsp[0].cstring))-2);
      (yyval.value) = new Value(tmp);
      free(tmp);
    }
#line 2383 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 66: /* value: NULL_T  */
#line 551 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            {
      (yyval.value) = new Value();
      (yyval.value)->set_null();
      (yyval.value)->set_type(AttrType::UNDEFINED);  // NULL值类型标识
      (yyloc) = (yylsp[0]);
    }
#line 2394 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 67: /* storage_format: %empty  */
#line 560 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.cstring) = nullptr;
    }
#line 2402 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 68: /* storage_format: STORAGE FORMAT EQ ID  */
#line 564 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.cstring) = (yyvsp[0].cstring);
    }
#line 2410 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 69: /* delete_stmt: DELETE FROM ID where  */
#line 571 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
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

  case 70: /* update_stmt: UPDATE ID SET update_list where  */
#line 582 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
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

  case 71: /* update_list: ID EQ expression  */
#line 598 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.update_list) = new UpdateList();
      (yyval.update_list)->attribute_names.push_back((yyvsp[-2].cstring));
      (yyval.update_list)->expressions.push_back((yyvsp[0].expression));
      // 不需要 free($1)，sql_parse 会统一清理 allocated_strings
    }
#line 2451 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 72: /* update_list: update_list COMMA ID EQ expression  */
#line 605 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.update_list) = (yyvsp[-4].update_list);
      (yyval.update_list)->attribute_names.push_back((yyvsp[-2].cstring));
      (yyval.update_list)->expressions.push_back((yyvsp[0].expression));
      // 不需要 free($3)，sql_parse 会统一清理 allocated_strings
    }
#line 2462 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 73: /* select_stmt: SELECT expression_list FROM rel_list join_list where group_by having  */
#line 615 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SELECT);
      if ((yyvsp[-6].expression_list) != nullptr) {
        (yyval.sql_node)->selection.expressions.swap(*(yyvsp[-6].expression_list));
        delete (yyvsp[-6].expression_list);
      }

      if ((yyvsp[-4].relation_list) != nullptr) {
        (yyval.sql_node)->selection.relations.swap(*(yyvsp[-4].relation_list));
        delete (yyvsp[-4].relation_list);
      }

      if ((yyvsp[-3].join_list) != nullptr) {
        (yyval.sql_node)->selection.joins.swap(*(yyvsp[-3].join_list));
        delete (yyvsp[-3].join_list);
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
#line 2499 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 74: /* select_stmt: SELECT expression_list WHERE condition_list  */
#line 648 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
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
#line 2517 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 75: /* select_stmt: SELECT expression_list  */
#line 662 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SELECT);
      if ((yyvsp[0].expression_list) != nullptr) {
        (yyval.sql_node)->selection.expressions.swap(*(yyvsp[0].expression_list));
        delete (yyvsp[0].expression_list);
      }
      // 不设置relations，表示没有FROM子句
    }
#line 2530 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 76: /* calc_stmt: CALC expression_list  */
#line 673 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CALC);
      (yyval.sql_node)->calc.expressions.swap(*(yyvsp[0].expression_list));
      delete (yyvsp[0].expression_list);
    }
#line 2540 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 77: /* expression_list: expression  */
#line 682 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.expression_list) = new vector<unique_ptr<Expression>>;
      (yyval.expression_list)->emplace_back((yyvsp[0].expression));
    }
#line 2549 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 78: /* expression_list: expression COMMA expression_list  */
#line 687 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      if ((yyvsp[0].expression_list) != nullptr) {
        (yyval.expression_list) = (yyvsp[0].expression_list);
      } else {
        (yyval.expression_list) = new vector<unique_ptr<Expression>>;
      }
      (yyval.expression_list)->emplace((yyval.expression_list)->begin(), (yyvsp[-2].expression));
    }
#line 2562 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 79: /* expression: expression '+' expression  */
#line 697 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                              {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::ADD, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2570 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 80: /* expression: expression '-' expression  */
#line 700 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::SUB, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2578 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 81: /* expression: expression '*' expression  */
#line 703 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::MUL, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2586 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 82: /* expression: expression '/' expression  */
#line 706 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                {
      printf("DEBUG: Creating DIV expression\n");
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::DIV, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2595 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 83: /* expression: LBRACE expression RBRACE  */
#line 710 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                               {
      (yyval.expression) = (yyvsp[-1].expression);
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
    }
#line 2604 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 84: /* expression: '-' expression  */
#line 714 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                  {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::NEGATIVE, (yyvsp[0].expression), nullptr, sql_string, &(yyloc));
    }
#line 2612 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 85: /* expression: value  */
#line 717 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
            {
      (yyval.expression) = new ValueExpr(*(yyvsp[0].value));
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
      delete (yyvsp[0].value);
    }
#line 2622 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 86: /* expression: rel_attr  */
#line 722 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
               {
      RelAttrSqlNode *node = (yyvsp[0].rel_attr);
      (yyval.expression) = new UnboundFieldExpr(node->relation_name, node->attribute_name);
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
      delete (yyvsp[0].rel_attr);
    }
#line 2633 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 87: /* expression: '*'  */
#line 728 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
          {
      (yyval.expression) = new StarExpr();
    }
#line 2641 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 88: /* expression: COUNT LBRACE '*' RBRACE  */
#line 731 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                              {
      (yyval.expression) = create_aggregate_expression("count", new StarExpr(), sql_string, &(yyloc));
    }
#line 2649 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 89: /* expression: COUNT LBRACE expression RBRACE  */
#line 734 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                     {
      (yyval.expression) = create_aggregate_expression("count", (yyvsp[-1].expression), sql_string, &(yyloc));
    }
#line 2657 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 90: /* expression: SUM LBRACE expression RBRACE  */
#line 737 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                   {
      (yyval.expression) = create_aggregate_expression("sum", (yyvsp[-1].expression), sql_string, &(yyloc));
    }
#line 2665 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 91: /* expression: AVG LBRACE expression RBRACE  */
#line 740 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                   {
      (yyval.expression) = create_aggregate_expression("avg", (yyvsp[-1].expression), sql_string, &(yyloc));
    }
#line 2673 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 92: /* expression: MAX LBRACE expression RBRACE  */
#line 743 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                   {
      (yyval.expression) = create_aggregate_expression("max", (yyvsp[-1].expression), sql_string, &(yyloc));
    }
#line 2681 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 93: /* expression: MIN LBRACE expression RBRACE  */
#line 746 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                   {
      (yyval.expression) = create_aggregate_expression("min", (yyvsp[-1].expression), sql_string, &(yyloc));
    }
#line 2689 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 94: /* expression: LBRACE select_stmt RBRACE  */
#line 749 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                {
      // 子查询表达式
      (yyval.expression) = new SubqueryExpr(SelectSqlNode::create_copy(&((yyvsp[-1].sql_node)->selection)));
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
      delete (yyvsp[-1].sql_node);
    }
#line 2700 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 95: /* expression: expression IN LBRACE expression_list RBRACE  */
#line 755 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                                  {
      // IN (value_list) 表达式
      vector<unique_ptr<Expression>> value_list;
      if ((yyvsp[-1].expression_list) != nullptr) {
        value_list = std::move(*(yyvsp[-1].expression_list));
        delete (yyvsp[-1].expression_list);
      }
      (yyval.expression) = new InExpr(false, unique_ptr<Expression>((yyvsp[-4].expression)), std::move(value_list));
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
    }
#line 2715 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 96: /* expression: expression NOT IN LBRACE expression_list RBRACE  */
#line 765 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                                      {
      // NOT IN (value_list) 表达式
      vector<unique_ptr<Expression>> value_list;
      if ((yyvsp[-1].expression_list) != nullptr) {
        value_list = std::move(*(yyvsp[-1].expression_list));
        delete (yyvsp[-1].expression_list);
      }
      (yyval.expression) = new InExpr(true, unique_ptr<Expression>((yyvsp[-5].expression)), std::move(value_list));
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
    }
#line 2730 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 97: /* expression: expression IN LBRACE select_stmt RBRACE  */
#line 775 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                              {
      // IN (SELECT ...) 表达式
      auto subquery = new SubqueryExpr(SelectSqlNode::create_copy(&((yyvsp[-1].sql_node)->selection)));
      (yyval.expression) = new InExpr(false, unique_ptr<Expression>((yyvsp[-4].expression)), unique_ptr<Expression>(subquery));
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
      delete (yyvsp[-1].sql_node);
    }
#line 2742 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 98: /* expression: expression NOT IN LBRACE select_stmt RBRACE  */
#line 782 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                                  {
      // NOT IN (SELECT ...) 表达式
      auto subquery = new SubqueryExpr(SelectSqlNode::create_copy(&((yyvsp[-1].sql_node)->selection)));
      (yyval.expression) = new InExpr(true, unique_ptr<Expression>((yyvsp[-5].expression)), unique_ptr<Expression>(subquery));
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
      delete (yyvsp[-1].sql_node);
    }
#line 2754 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 99: /* expression: EXISTS LBRACE select_stmt RBRACE  */
#line 789 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                       {
      // EXISTS (SELECT ...) 表达式
      auto subquery = new SubqueryExpr(SelectSqlNode::create_copy(&((yyvsp[-1].sql_node)->selection)));
      (yyval.expression) = new ExistsExpr(false, unique_ptr<Expression>(subquery));
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
      delete (yyvsp[-1].sql_node);
    }
#line 2766 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 100: /* expression: NOT EXISTS LBRACE select_stmt RBRACE  */
#line 796 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                           {
      // NOT EXISTS (SELECT ...) 表达式
      auto subquery = new SubqueryExpr(SelectSqlNode::create_copy(&((yyvsp[-1].sql_node)->selection)));
      (yyval.expression) = new ExistsExpr(true, unique_ptr<Expression>(subquery));
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
      delete (yyvsp[-1].sql_node);
    }
#line 2778 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 101: /* rel_attr: ID  */
#line 806 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
       {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->attribute_name = (yyvsp[0].cstring);
    }
#line 2787 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 102: /* rel_attr: ID DOT ID  */
#line 810 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->relation_name  = (yyvsp[-2].cstring);
      (yyval.rel_attr)->attribute_name = (yyvsp[0].cstring);
    }
#line 2797 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 103: /* relation: ID  */
#line 818 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
       {
      (yyval.cstring) = (yyvsp[0].cstring);
    }
#line 2805 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 104: /* rel_list: relation  */
#line 823 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
             {
      (yyval.relation_list) = new vector<string>();
      (yyval.relation_list)->push_back((yyvsp[0].cstring));
    }
#line 2814 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 105: /* rel_list: relation COMMA rel_list  */
#line 827 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                              {
      if ((yyvsp[0].relation_list) != nullptr) {
        (yyval.relation_list) = (yyvsp[0].relation_list);
      } else {
        (yyval.relation_list) = new vector<string>;
      }

      (yyval.relation_list)->insert((yyval.relation_list)->begin(), (yyvsp[-2].cstring));
    }
#line 2828 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 106: /* where: %empty  */
#line 840 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.condition_list) = nullptr;
    }
#line 2836 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 107: /* where: WHERE condition_list  */
#line 843 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                           {
      (yyval.condition_list) = (yyvsp[0].condition_list);  
    }
#line 2844 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 108: /* having: %empty  */
#line 849 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.condition_list) = nullptr;
    }
#line 2852 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 109: /* having: HAVING condition_list  */
#line 852 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                            {
      (yyval.condition_list) = (yyvsp[0].condition_list);
    }
#line 2860 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 110: /* condition_list: %empty  */
#line 858 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.condition_list) = nullptr;
    }
#line 2868 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 111: /* condition_list: condition  */
#line 861 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                {
      (yyval.condition_list) = new vector<ConditionSqlNode>;
      (yyval.condition_list)->push_back(*(yyvsp[0].condition));
      delete (yyvsp[0].condition);
    }
#line 2878 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 112: /* condition_list: condition AND condition_list  */
#line 866 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                   {
      if ((yyvsp[0].condition_list) == nullptr) {
        (yyval.condition_list) = new vector<ConditionSqlNode>;
      } else {
        (yyval.condition_list) = (yyvsp[0].condition_list);
      }
      (yyval.condition_list)->insert((yyval.condition_list)->begin(), *(yyvsp[-2].condition));
      delete (yyvsp[-2].condition);
    }
#line 2892 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 113: /* condition: expression comp_op expression  */
#line 878 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
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
#line 2909 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 114: /* condition: expression IS NULL_T  */
#line 891 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
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
#line 2926 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 115: /* condition: expression IS NOT NULL_T  */
#line 904 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
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
#line 2943 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 116: /* condition: expression  */
#line 917 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
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
#line 2960 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 117: /* comp_op: EQ  */
#line 932 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
         { (yyval.comp) = EQUAL_TO; }
#line 2966 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 118: /* comp_op: LT  */
#line 933 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
         { (yyval.comp) = LESS_THAN; }
#line 2972 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 119: /* comp_op: GT  */
#line 934 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
         { (yyval.comp) = GREAT_THAN; }
#line 2978 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 120: /* comp_op: LE  */
#line 935 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
         { (yyval.comp) = LESS_EQUAL; }
#line 2984 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 121: /* comp_op: GE  */
#line 936 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
         { (yyval.comp) = GREAT_EQUAL; }
#line 2990 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 122: /* comp_op: NE  */
#line 937 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
         { (yyval.comp) = NOT_EQUAL; }
#line 2996 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 123: /* comp_op: LIKE  */
#line 938 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
           { (yyval.comp) = LIKE_OP; }
#line 3002 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 124: /* comp_op: NOT LIKE  */
#line 939 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
               { (yyval.comp) = NOT_LIKE_OP; }
#line 3008 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 125: /* on_conditions: expression comp_op expression  */
#line 944 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                  {
      (yyval.condition_list) = new vector<ConditionSqlNode>;
      ConditionSqlNode *cond = new ConditionSqlNode;
      cond->comp = (yyvsp[-1].comp);
      cond->left_expression = (yyvsp[-2].expression);
      cond->right_expression = (yyvsp[0].expression);
      cond->is_expression_condition = true;
      cond->left_is_attr = 0;
      cond->right_is_attr = 0;
      (yyval.condition_list)->push_back(*cond);
      delete cond;
    }
#line 3025 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 126: /* on_conditions: expression comp_op expression AND on_conditions  */
#line 956 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
                                                      {
      if ((yyvsp[0].condition_list) == nullptr) {
        (yyval.condition_list) = new vector<ConditionSqlNode>;
      } else {
        (yyval.condition_list) = (yyvsp[0].condition_list);
      }
      ConditionSqlNode cond;
      cond.comp = (yyvsp[-3].comp);
      cond.left_expression = (yyvsp[-4].expression);
      cond.right_expression = (yyvsp[-2].expression);
      cond.is_expression_condition = true;
      cond.left_is_attr = 0;
      cond.right_is_attr = 0;
      (yyval.condition_list)->insert((yyval.condition_list)->begin(), cond);
    }
#line 3045 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 127: /* join_list: %empty  */
#line 976 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.join_list) = nullptr;
    }
#line 3053 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 128: /* join_list: INNER JOIN relation ON on_conditions  */
#line 980 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.join_list) = new vector<JoinSqlNode>;
      JoinSqlNode join_node;
      join_node.type = JoinType::INNER_JOIN;
      join_node.relation = (yyvsp[-2].cstring);
      
      // 复制所有ON条件
      if ((yyvsp[0].condition_list) != nullptr) {
        join_node.conditions = *(yyvsp[0].condition_list);
        delete (yyvsp[0].condition_list);
      }
      
      (yyval.join_list)->push_back(join_node);
    }
#line 3072 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 129: /* join_list: join_list INNER JOIN relation ON on_conditions  */
#line 995 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      if ((yyvsp[-5].join_list) != nullptr) {
        (yyval.join_list) = (yyvsp[-5].join_list);
      } else {
        (yyval.join_list) = new vector<JoinSqlNode>;
      }
      
      JoinSqlNode join_node;
      join_node.type = JoinType::INNER_JOIN;
      join_node.relation = (yyvsp[-2].cstring);
      
      // 复制所有ON条件
      if ((yyvsp[0].condition_list) != nullptr) {
        join_node.conditions = *(yyvsp[0].condition_list);
        delete (yyvsp[0].condition_list);
      }
      
      (yyval.join_list)->push_back(join_node);
    }
#line 3096 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 130: /* group_by: %empty  */
#line 1018 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.expression_list) = nullptr;
    }
#line 3104 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 131: /* group_by: GROUP BY expression_list  */
#line 1022 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.expression_list) = (yyvsp[0].expression_list); 
    }
#line 3112 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 132: /* load_data_stmt: LOAD DATA INFILE SSS INTO TABLE ID  */
#line 1028 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      char *tmp_file_name = common::substr((yyvsp[-3].cstring), 1, strlen((yyvsp[-3].cstring)) - 2);
      
      (yyval.sql_node) = new ParsedSqlNode(SCF_LOAD_DATA);
      (yyval.sql_node)->load_data.relation_name = (yyvsp[0].cstring);
      (yyval.sql_node)->load_data.file_name = tmp_file_name;
      free(tmp_file_name);
    }
#line 3125 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 133: /* explain_stmt: EXPLAIN command_wrapper  */
#line 1040 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_EXPLAIN);
      (yyval.sql_node)->explain.sql_node = unique_ptr<ParsedSqlNode>((yyvsp[0].sql_node));
    }
#line 3134 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 134: /* set_variable_stmt: SET ID EQ value  */
#line 1048 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SET_VARIABLE);
      (yyval.sql_node)->set_variable.name  = (yyvsp[-2].cstring);
      (yyval.sql_node)->set_variable.value = *(yyvsp[0].value);
      delete (yyvsp[0].value);
    }
#line 3145 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"
    break;


#line 3149 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.cpp"

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

#line 1059 "/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y"

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

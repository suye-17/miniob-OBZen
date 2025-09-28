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
#line 2 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"


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


#line 151 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"

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
  YYSYMBOL_CALC = 14,                      /* CALC  */
  YYSYMBOL_SELECT = 15,                    /* SELECT  */
  YYSYMBOL_DESC = 16,                      /* DESC  */
  YYSYMBOL_SHOW = 17,                      /* SHOW  */
  YYSYMBOL_SYNC = 18,                      /* SYNC  */
  YYSYMBOL_INSERT = 19,                    /* INSERT  */
  YYSYMBOL_DELETE = 20,                    /* DELETE  */
  YYSYMBOL_UPDATE = 21,                    /* UPDATE  */
  YYSYMBOL_LBRACE = 22,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 23,                    /* RBRACE  */
  YYSYMBOL_COMMA = 24,                     /* COMMA  */
  YYSYMBOL_TRX_BEGIN = 25,                 /* TRX_BEGIN  */
  YYSYMBOL_TRX_COMMIT = 26,                /* TRX_COMMIT  */
  YYSYMBOL_TRX_ROLLBACK = 27,              /* TRX_ROLLBACK  */
  YYSYMBOL_INT_T = 28,                     /* INT_T  */
  YYSYMBOL_STRING_T = 29,                  /* STRING_T  */
  YYSYMBOL_FLOAT_T = 30,                   /* FLOAT_T  */
  YYSYMBOL_DATE_T = 31,                    /* DATE_T  */
  YYSYMBOL_NULL_T = 32,                    /* NULL_T  */
  YYSYMBOL_NOT = 33,                       /* NOT  */
  YYSYMBOL_IS = 34,                        /* IS  */
  YYSYMBOL_VECTOR_T = 35,                  /* VECTOR_T  */
  YYSYMBOL_HELP = 36,                      /* HELP  */
  YYSYMBOL_EXIT = 37,                      /* EXIT  */
  YYSYMBOL_DOT = 38,                       /* DOT  */
  YYSYMBOL_INTO = 39,                      /* INTO  */
  YYSYMBOL_VALUES = 40,                    /* VALUES  */
  YYSYMBOL_FROM = 41,                      /* FROM  */
  YYSYMBOL_WHERE = 42,                     /* WHERE  */
  YYSYMBOL_AND = 43,                       /* AND  */
  YYSYMBOL_SET = 44,                       /* SET  */
  YYSYMBOL_ON = 45,                        /* ON  */
  YYSYMBOL_LOAD = 46,                      /* LOAD  */
  YYSYMBOL_DATA = 47,                      /* DATA  */
  YYSYMBOL_INFILE = 48,                    /* INFILE  */
  YYSYMBOL_EXPLAIN = 49,                   /* EXPLAIN  */
  YYSYMBOL_STORAGE = 50,                   /* STORAGE  */
  YYSYMBOL_FORMAT = 51,                    /* FORMAT  */
  YYSYMBOL_PRIMARY = 52,                   /* PRIMARY  */
  YYSYMBOL_KEY = 53,                       /* KEY  */
  YYSYMBOL_ANALYZE = 54,                   /* ANALYZE  */
  YYSYMBOL_COUNT = 55,                     /* COUNT  */
  YYSYMBOL_SUM = 56,                       /* SUM  */
  YYSYMBOL_AVG = 57,                       /* AVG  */
  YYSYMBOL_MAX = 58,                       /* MAX  */
  YYSYMBOL_MIN = 59,                       /* MIN  */
  YYSYMBOL_EQ = 60,                        /* EQ  */
  YYSYMBOL_LT = 61,                        /* LT  */
  YYSYMBOL_GT = 62,                        /* GT  */
  YYSYMBOL_LE = 63,                        /* LE  */
  YYSYMBOL_GE = 64,                        /* GE  */
  YYSYMBOL_NE = 65,                        /* NE  */
  YYSYMBOL_NUMBER = 66,                    /* NUMBER  */
  YYSYMBOL_FLOAT = 67,                     /* FLOAT  */
  YYSYMBOL_ID = 68,                        /* ID  */
  YYSYMBOL_SSS = 69,                       /* SSS  */
  YYSYMBOL_70_ = 70,                       /* '+'  */
  YYSYMBOL_71_ = 71,                       /* '-'  */
  YYSYMBOL_72_ = 72,                       /* '*'  */
  YYSYMBOL_73_ = 73,                       /* '/'  */
  YYSYMBOL_UMINUS = 74,                    /* UMINUS  */
  YYSYMBOL_YYACCEPT = 75,                  /* $accept  */
  YYSYMBOL_commands = 76,                  /* commands  */
  YYSYMBOL_command_wrapper = 77,           /* command_wrapper  */
  YYSYMBOL_exit_stmt = 78,                 /* exit_stmt  */
  YYSYMBOL_help_stmt = 79,                 /* help_stmt  */
  YYSYMBOL_sync_stmt = 80,                 /* sync_stmt  */
  YYSYMBOL_begin_stmt = 81,                /* begin_stmt  */
  YYSYMBOL_commit_stmt = 82,               /* commit_stmt  */
  YYSYMBOL_rollback_stmt = 83,             /* rollback_stmt  */
  YYSYMBOL_drop_table_stmt = 84,           /* drop_table_stmt  */
  YYSYMBOL_analyze_table_stmt = 85,        /* analyze_table_stmt  */
  YYSYMBOL_show_tables_stmt = 86,          /* show_tables_stmt  */
  YYSYMBOL_desc_table_stmt = 87,           /* desc_table_stmt  */
  YYSYMBOL_create_index_stmt = 88,         /* create_index_stmt  */
  YYSYMBOL_attribute_name_list = 89,       /* attribute_name_list  */
  YYSYMBOL_drop_index_stmt = 90,           /* drop_index_stmt  */
  YYSYMBOL_show_index_stmt = 91,           /* show_index_stmt  */
  YYSYMBOL_create_table_stmt = 92,         /* create_table_stmt  */
  YYSYMBOL_attr_def_list = 93,             /* attr_def_list  */
  YYSYMBOL_attr_def = 94,                  /* attr_def  */
  YYSYMBOL_nullable_spec = 95,             /* nullable_spec  */
  YYSYMBOL_number = 96,                    /* number  */
  YYSYMBOL_type = 97,                      /* type  */
  YYSYMBOL_primary_key = 98,               /* primary_key  */
  YYSYMBOL_attr_list = 99,                 /* attr_list  */
  YYSYMBOL_insert_stmt = 100,              /* insert_stmt  */
  YYSYMBOL_value_list = 101,               /* value_list  */
  YYSYMBOL_value = 102,                    /* value  */
  YYSYMBOL_storage_format = 103,           /* storage_format  */
  YYSYMBOL_delete_stmt = 104,              /* delete_stmt  */
  YYSYMBOL_update_stmt = 105,              /* update_stmt  */
  YYSYMBOL_select_stmt = 106,              /* select_stmt  */
  YYSYMBOL_calc_stmt = 107,                /* calc_stmt  */
  YYSYMBOL_expression_list = 108,          /* expression_list  */
  YYSYMBOL_expression = 109,               /* expression  */
  YYSYMBOL_rel_attr = 110,                 /* rel_attr  */
  YYSYMBOL_relation = 111,                 /* relation  */
  YYSYMBOL_rel_list = 112,                 /* rel_list  */
  YYSYMBOL_where = 113,                    /* where  */
  YYSYMBOL_having = 114,                   /* having  */
  YYSYMBOL_condition_list = 115,           /* condition_list  */
  YYSYMBOL_condition = 116,                /* condition  */
  YYSYMBOL_comp_op = 117,                  /* comp_op  */
  YYSYMBOL_group_by = 118,                 /* group_by  */
  YYSYMBOL_load_data_stmt = 119,           /* load_data_stmt  */
  YYSYMBOL_explain_stmt = 120,             /* explain_stmt  */
  YYSYMBOL_set_variable_stmt = 121,        /* set_variable_stmt  */
  YYSYMBOL_opt_semicolon = 122             /* opt_semicolon  */
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
typedef yytype_uint8 yy_state_t;

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
#define YYFINAL  76
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   232

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  75
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  48
/* YYNRULES -- Number of rules.  */
#define YYNRULES  124
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  227

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   325


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
      65,    66,    67,    68,    69,    74
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   252,   252,   260,   261,   262,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   275,   276,
     277,   278,   279,   280,   281,   285,   291,   296,   302,   308,
     314,   320,   326,   333,   339,   346,   357,   362,   369,   377,
     384,   405,   411,   420,   428,   436,   444,   455,   456,   457,
     461,   464,   465,   466,   467,   468,   472,   475,   482,   486,
     498,   508,   514,   521,   525,   529,   534,   543,   546,   553,
     564,   577,   605,   619,   630,   639,   644,   655,   658,   661,
     664,   668,   672,   675,   680,   686,   689,   692,   695,   698,
     701,   704,   707,   710,   713,   716,   722,   726,   734,   739,
     743,   756,   759,   765,   768,   774,   777,   782,   789,   802,
     815,   828,   844,   845,   846,   847,   848,   849,   855,   858,
     864,   876,   884,   893,   894
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
  "INDEX", "CALC", "SELECT", "DESC", "SHOW", "SYNC", "INSERT", "DELETE",
  "UPDATE", "LBRACE", "RBRACE", "COMMA", "TRX_BEGIN", "TRX_COMMIT",
  "TRX_ROLLBACK", "INT_T", "STRING_T", "FLOAT_T", "DATE_T", "NULL_T",
  "NOT", "IS", "VECTOR_T", "HELP", "EXIT", "DOT", "INTO", "VALUES", "FROM",
  "WHERE", "AND", "SET", "ON", "LOAD", "DATA", "INFILE", "EXPLAIN",
  "STORAGE", "FORMAT", "PRIMARY", "KEY", "ANALYZE", "COUNT", "SUM", "AVG",
  "MAX", "MIN", "EQ", "LT", "GT", "LE", "GE", "NE", "NUMBER", "FLOAT",
  "ID", "SSS", "'+'", "'-'", "'*'", "'/'", "UMINUS", "$accept", "commands",
  "command_wrapper", "exit_stmt", "help_stmt", "sync_stmt", "begin_stmt",
  "commit_stmt", "rollback_stmt", "drop_table_stmt", "analyze_table_stmt",
  "show_tables_stmt", "desc_table_stmt", "create_index_stmt",
  "attribute_name_list", "drop_index_stmt", "show_index_stmt",
  "create_table_stmt", "attr_def_list", "attr_def", "nullable_spec",
  "number", "type", "primary_key", "attr_list", "insert_stmt",
  "value_list", "value", "storage_format", "delete_stmt", "update_stmt",
  "select_stmt", "calc_stmt", "expression_list", "expression", "rel_attr",
  "relation", "rel_list", "where", "having", "condition_list", "condition",
  "comp_op", "group_by", "load_data_stmt", "explain_stmt",
  "set_variable_stmt", "opt_semicolon", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-154)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     122,     0,    10,    -2,    -2,   -59,    -5,  -154,   -21,   -19,
     -36,  -154,  -154,  -154,  -154,  -154,   -17,    15,   122,    41,
      68,    92,  -154,  -154,  -154,  -154,  -154,  -154,  -154,  -154,
    -154,  -154,  -154,  -154,  -154,  -154,  -154,  -154,  -154,  -154,
    -154,  -154,  -154,  -154,    46,    56,    63,    76,    -2,  -154,
     123,   124,   130,   133,   135,  -154,  -154,    71,  -154,    -2,
    -154,  -154,  -154,    55,  -154,   -27,  -154,  -154,   119,    93,
      94,   120,   103,   121,  -154,    99,  -154,  -154,  -154,   143,
     125,  -154,   127,    26,    -2,    -2,    -2,    -2,    -2,   105,
    -154,    -2,    -2,    -2,    -2,    -2,   106,    -2,  -154,   107,
     144,   141,   118,    87,   116,  -154,   126,   128,   129,  -154,
     168,     1,   170,     5,   172,    19,   175,    35,   176,    62,
    -154,  -154,   -46,   -46,  -154,  -154,  -154,   177,   141,   117,
    -154,   157,  -154,   180,  -154,   145,  -154,   164,    82,   182,
    -154,   185,  -154,  -154,  -154,  -154,  -154,  -154,  -154,  -154,
    -154,  -154,  -154,   106,   195,    28,  -154,  -154,  -154,  -154,
    -154,  -154,    -2,    -2,    87,    -2,   197,  -154,  -154,  -154,
    -154,  -154,   -16,   -49,   186,   142,  -154,   207,   202,  -154,
     181,    31,  -154,    70,  -154,   -37,   146,   149,  -154,   184,
    -154,   165,  -154,   167,  -154,    97,    -2,    -2,  -154,  -154,
    -154,    87,  -154,  -154,  -154,   196,  -154,   198,   171,  -154,
    -154,   153,  -154,  -154,  -154,    90,   155,   166,  -154,  -154,
     200,   204,   160,   155,  -154,  -154,  -154
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,    27,     0,     0,
       0,    28,    29,    30,    26,    25,     0,     0,     0,     0,
       0,   123,    24,    23,    16,    17,    18,    19,     9,    10,
      11,    13,    14,    15,    12,     8,     5,     7,     6,     4,
       3,    20,    21,    22,     0,     0,     0,     0,     0,    66,
       0,     0,     0,     0,     0,    63,    64,    96,    65,     0,
      85,    83,    74,    75,    84,    73,    34,    33,     0,     0,
       0,     0,     0,     0,   121,     0,     1,   124,     2,     0,
       0,    31,     0,     0,     0,     0,     0,     0,     0,     0,
      82,     0,     0,     0,     0,     0,     0,   105,    72,     0,
       0,   101,     0,     0,     0,    32,     0,     0,     0,    81,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      97,    76,    77,    78,    79,    80,    98,    99,   101,   111,
     102,   106,    39,     0,    69,     0,   122,     0,     0,    56,
      41,     0,    38,    87,    86,    89,    88,    91,    90,    93,
      92,    95,    94,     0,   118,     0,   112,   113,   114,   115,
     116,   117,     0,   105,     0,     0,     0,    51,    52,    53,
      55,    54,    46,     0,     0,     0,   100,     0,   103,   109,
       0,   108,   107,     0,    61,   101,     0,     0,    47,     0,
      44,     0,    42,    67,    36,     0,     0,   105,    71,   110,
      60,     0,    70,   120,    50,     0,    48,     0,     0,    40,
      35,     0,   119,   104,    62,    45,     0,     0,    37,    43,
      58,     0,     0,     0,    57,    68,    59
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -154,  -154,   211,  -154,  -154,  -154,  -154,  -154,  -154,  -154,
    -154,  -154,  -154,  -154,  -154,  -154,  -154,  -154,  -154,    52,
      16,  -154,  -154,  -154,     7,  -154,  -154,  -101,  -154,  -154,
    -154,  -154,  -154,    -4,   -47,  -154,  -154,    79,   -97,  -154,
    -153,  -154,  -154,  -154,  -154,  -154,  -154,  -154
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,   195,    33,    34,    35,   139,   140,
     190,   205,   172,   174,   221,    36,   183,    61,   209,    37,
      38,    39,    40,    62,    63,    64,   127,   128,    98,   198,
     130,   131,   162,   178,    41,    42,    43,    78
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      65,    83,   136,   191,   134,    97,   187,    67,    68,    66,
     182,    44,    90,    45,    96,    97,   188,   189,    69,   138,
      48,    46,    70,    47,   144,    91,    94,    95,   146,    91,
      49,   154,    71,    92,    93,    94,    95,   111,   113,   115,
     117,   119,   148,    91,   213,   122,   123,   124,   125,   109,
     129,    72,    75,    50,    51,    52,    53,    54,   150,    91,
     179,   180,    73,   184,    55,    56,    57,    58,    76,    59,
      60,    92,    93,    94,    95,    92,    93,    94,    95,    91,
     110,   112,   114,   116,   118,   152,    91,   121,   202,    92,
      93,    94,    95,   200,   201,    77,    92,    93,    94,    95,
     214,    92,    93,    94,    95,    92,    93,    94,    95,    89,
     167,   168,   169,   170,    79,   181,   129,   171,   185,    49,
     210,   211,   188,   189,    80,    92,    93,    94,    95,     1,
       2,    81,    92,    93,    94,    95,     3,     4,     5,     6,
       7,     8,     9,    10,    82,    84,    85,    11,    12,    13,
     129,   155,    86,    55,    56,    87,    58,    88,    14,    15,
      99,   100,   101,   103,   102,   106,    16,   105,    17,   104,
     107,    18,   108,   120,   126,   132,    19,   156,   157,   158,
     159,   160,   161,    97,   133,   137,   135,    92,    93,    94,
      95,   143,   212,   145,   138,   147,   141,   142,   149,   151,
     163,   153,   164,   166,   177,   165,   173,   175,   186,   193,
     194,   196,   197,   199,   203,   204,   206,   208,   207,   215,
     216,   218,   217,   220,   223,   192,   222,   224,   225,    74,
     226,   219,   176
};

static const yytype_uint8 yycheck[] =
{
       4,    48,   103,    52,   101,    42,    22,    12,    13,    68,
     163,    11,    59,    13,    41,    42,    32,    33,    39,    68,
      22,    11,    41,    13,    23,    24,    72,    73,    23,    24,
      32,   128,    68,    70,    71,    72,    73,    84,    85,    86,
      87,    88,    23,    24,   197,    92,    93,    94,    95,    23,
      97,    68,    11,    55,    56,    57,    58,    59,    23,    24,
      32,    33,    47,   164,    66,    67,    68,    69,     0,    71,
      72,    70,    71,    72,    73,    70,    71,    72,    73,    24,
      84,    85,    86,    87,    88,    23,    24,    91,   185,    70,
      71,    72,    73,    23,    24,     3,    70,    71,    72,    73,
     201,    70,    71,    72,    73,    70,    71,    72,    73,    38,
      28,    29,    30,    31,    68,   162,   163,    35,   165,    32,
      23,    24,    32,    33,    68,    70,    71,    72,    73,     7,
       8,    68,    70,    71,    72,    73,    14,    15,    16,    17,
      18,    19,    20,    21,    68,    22,    22,    25,    26,    27,
     197,    34,    22,    66,    67,    22,    69,    22,    36,    37,
      41,    68,    68,    60,    44,    22,    44,    68,    46,    48,
      45,    49,    45,    68,    68,    68,    54,    60,    61,    62,
      63,    64,    65,    42,    40,    69,    68,    70,    71,    72,
      73,    23,   196,    23,    68,    23,    68,    68,    23,    23,
      43,    24,    22,    39,     9,    60,    24,    22,    11,    23,
      68,     4,    10,    32,    68,    66,    32,    50,    53,    23,
      22,    68,    51,    68,    24,   173,    60,    23,    68,    18,
     223,   215,   153
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     7,     8,    14,    15,    16,    17,    18,    19,    20,
      21,    25,    26,    27,    36,    37,    44,    46,    49,    54,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    90,    91,    92,   100,   104,   105,   106,
     107,   119,   120,   121,    11,    13,    11,    13,    22,    32,
      55,    56,    57,    58,    59,    66,    67,    68,    69,    71,
      72,   102,   108,   109,   110,   108,    68,    12,    13,    39,
      41,    68,    68,    47,    77,    11,     0,     3,   122,    68,
      68,    68,    68,   109,    22,    22,    22,    22,    22,    38,
     109,    24,    70,    71,    72,    73,    41,    42,   113,    41,
      68,    68,    44,    60,    48,    68,    22,    45,    45,    23,
     108,   109,   108,   109,   108,   109,   108,   109,   108,   109,
      68,   108,   109,   109,   109,   109,    68,   111,   112,   109,
     115,   116,    68,    40,   113,    68,   102,    69,    68,    93,
      94,    68,    68,    23,    23,    23,    23,    23,    23,    23,
      23,    23,    23,    24,   113,    34,    60,    61,    62,    63,
      64,    65,   117,    43,    22,    60,    39,    28,    29,    30,
      31,    35,    97,    24,    98,    22,   112,     9,   118,    32,
      33,   109,   115,   101,   102,   109,    11,    22,    32,    33,
      95,    52,    94,    23,    68,    89,     4,    10,   114,    32,
      23,    24,   113,    68,    66,    96,    32,    53,    50,   103,
      23,    24,   108,   115,   102,    23,    22,    51,    68,    95,
      68,    99,    60,    24,    23,    68,    99
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    75,    76,    77,    77,    77,    77,    77,    77,    77,
      77,    77,    77,    77,    77,    77,    77,    77,    77,    77,
      77,    77,    77,    77,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    89,    90,    91,
      92,    93,    93,    94,    94,    94,    94,    95,    95,    95,
      96,    97,    97,    97,    97,    97,    98,    98,    99,    99,
     100,   101,   101,   102,   102,   102,   102,   103,   103,   104,
     105,   106,   106,   106,   107,   108,   108,   109,   109,   109,
     109,   109,   109,   109,   109,   109,   109,   109,   109,   109,
     109,   109,   109,   109,   109,   109,   110,   110,   111,   112,
     112,   113,   113,   114,   114,   115,   115,   115,   116,   116,
     116,   116,   117,   117,   117,   117,   117,   117,   118,   118,
     119,   120,   121,   122,   122
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     2,     2,     8,     1,     3,     5,     4,
       8,     1,     3,     6,     3,     5,     2,     1,     2,     0,
       1,     1,     1,     1,     1,     1,     0,     6,     1,     3,
       7,     1,     3,     1,     1,     1,     1,     0,     4,     4,
       7,     7,     3,     2,     2,     1,     3,     3,     3,     3,
       3,     3,     2,     1,     1,     1,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     1,     3,     1,     1,
       3,     0,     2,     0,     2,     0,     1,     3,     3,     3,
       4,     1,     1,     1,     1,     1,     1,     1,     0,     3,
       7,     2,     4,     0,     1
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
#line 189 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).key_list); }
#line 1533 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_attr_def_list: /* attr_def_list  */
#line 181 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).attr_infos); }
#line 1539 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_attr_def: /* attr_def  */
#line 182 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).attr_info); }
#line 1545 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_primary_key: /* primary_key  */
#line 189 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).key_list); }
#line 1551 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_attr_list: /* attr_list  */
#line 189 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).key_list); }
#line 1557 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_value_list: /* value_list  */
#line 185 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).value_list); }
#line 1563 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_value: /* value  */
#line 179 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).value); }
#line 1569 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_expression_list: /* expression_list  */
#line 184 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).expression_list); }
#line 1575 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_expression: /* expression  */
#line 183 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).expression); }
#line 1581 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_rel_attr: /* rel_attr  */
#line 180 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).rel_attr); }
#line 1587 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_rel_list: /* rel_list  */
#line 188 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).relation_list); }
#line 1593 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_where: /* where  */
#line 186 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).condition_list); }
#line 1599 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_having: /* having  */
#line 186 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).condition_list); }
#line 1605 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_condition_list: /* condition_list  */
#line 186 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).condition_list); }
#line 1611 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_condition: /* condition  */
#line 178 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).condition); }
#line 1617 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
        break;

    case YYSYMBOL_group_by: /* group_by  */
#line 184 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
            { delete ((*yyvaluep).expression_list); }
#line 1623 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
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
#line 253 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
  {
    unique_ptr<ParsedSqlNode> sql_node = unique_ptr<ParsedSqlNode>((yyvsp[-1].sql_node));
    sql_result->add_sql_node(std::move(sql_node));
  }
#line 1932 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 25: /* exit_stmt: EXIT  */
#line 285 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
         {
      (void)yynerrs;  // 这么写为了消除yynerrs未使用的告警。如果你有更好的方法欢迎提PR
      (yyval.sql_node) = new ParsedSqlNode(SCF_EXIT);
    }
#line 1941 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 26: /* help_stmt: HELP  */
#line 291 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
         {
      (yyval.sql_node) = new ParsedSqlNode(SCF_HELP);
    }
#line 1949 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 27: /* sync_stmt: SYNC  */
#line 296 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
         {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SYNC);
    }
#line 1957 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 28: /* begin_stmt: TRX_BEGIN  */
#line 302 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
               {
      (yyval.sql_node) = new ParsedSqlNode(SCF_BEGIN);
    }
#line 1965 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 29: /* commit_stmt: TRX_COMMIT  */
#line 308 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
               {
      (yyval.sql_node) = new ParsedSqlNode(SCF_COMMIT);
    }
#line 1973 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 30: /* rollback_stmt: TRX_ROLLBACK  */
#line 314 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                  {
      (yyval.sql_node) = new ParsedSqlNode(SCF_ROLLBACK);
    }
#line 1981 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 31: /* drop_table_stmt: DROP TABLE ID  */
#line 320 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                  {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DROP_TABLE);
      (yyval.sql_node)->drop_table.relation_name = (yyvsp[0].cstring);
    }
#line 1990 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 32: /* analyze_table_stmt: ANALYZE TABLE ID  */
#line 326 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                     {
      (yyval.sql_node) = new ParsedSqlNode(SCF_ANALYZE_TABLE);
      (yyval.sql_node)->analyze_table.relation_name = (yyvsp[0].cstring);
    }
#line 1999 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 33: /* show_tables_stmt: SHOW TABLES  */
#line 333 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SHOW_TABLES);
    }
#line 2007 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 34: /* desc_table_stmt: DESC ID  */
#line 339 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
             {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DESC_TABLE);
      (yyval.sql_node)->desc_table.relation_name = (yyvsp[0].cstring);
    }
#line 2016 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 35: /* create_index_stmt: CREATE INDEX ID ON ID LBRACE attribute_name_list RBRACE  */
#line 347 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_INDEX);
      CreateIndexSqlNode &create_index = (yyval.sql_node)->create_index;
      create_index.index_name = (yyvsp[-5].cstring);
      create_index.relation_name = (yyvsp[-3].cstring);
      create_index.attribute_names = std::move (*(yyvsp[-1].key_list));
      delete (yyvsp[-1].key_list);
    }
#line 2029 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 36: /* attribute_name_list: ID  */
#line 358 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.key_list) = new vector<string> ();
      (yyval.key_list)->push_back((yyvsp[0].cstring));
    }
#line 2038 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 37: /* attribute_name_list: attribute_name_list COMMA ID  */
#line 363 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.key_list) = (yyvsp[-2].key_list);
      (yyval.key_list)->push_back((yyvsp[0].cstring));
    }
#line 2047 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 38: /* drop_index_stmt: DROP INDEX ID ON ID  */
#line 370 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DROP_INDEX);
      (yyval.sql_node)->drop_index.index_name = (yyvsp[-2].cstring);
      (yyval.sql_node)->drop_index.relation_name = (yyvsp[0].cstring);
    }
#line 2057 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 39: /* show_index_stmt: SHOW INDEX FROM ID  */
#line 378 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SHOW_INDEX);
      (yyval.sql_node)->show_index.relation_name = (yyvsp[0].cstring);  
    }
#line 2066 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 40: /* create_table_stmt: CREATE TABLE ID LBRACE attr_def_list primary_key RBRACE storage_format  */
#line 385 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
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
#line 2088 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 41: /* attr_def_list: attr_def  */
#line 406 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.attr_infos) = new vector<AttrInfoSqlNode>;
      (yyval.attr_infos)->emplace_back(*(yyvsp[0].attr_info));
      delete (yyvsp[0].attr_info);
    }
#line 2098 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 42: /* attr_def_list: attr_def_list COMMA attr_def  */
#line 412 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.attr_infos) = (yyvsp[-2].attr_infos);
      (yyval.attr_infos)->emplace_back(*(yyvsp[0].attr_info));
      delete (yyvsp[0].attr_info);
    }
#line 2108 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 43: /* attr_def: ID type LBRACE number RBRACE nullable_spec  */
#line 421 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-4].number);
      (yyval.attr_info)->name = (yyvsp[-5].cstring);
      (yyval.attr_info)->length = (yyvsp[-2].number);
      (yyval.attr_info)->nullable = ((yyvsp[0].number) == 1);
    }
#line 2120 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 44: /* attr_def: ID type nullable_spec  */
#line 429 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-1].number);
      (yyval.attr_info)->name = (yyvsp[-2].cstring);
      (yyval.attr_info)->length = 4;
      (yyval.attr_info)->nullable = ((yyvsp[0].number) == 1);
    }
#line 2132 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 45: /* attr_def: ID type LBRACE number RBRACE  */
#line 437 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-3].number);
      (yyval.attr_info)->name = (yyvsp[-4].cstring);
      (yyval.attr_info)->length = (yyvsp[-1].number);
      (yyval.attr_info)->nullable = true;  
    }
#line 2144 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 46: /* attr_def: ID type  */
#line 445 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[0].number);
      (yyval.attr_info)->name = (yyvsp[-1].cstring);
      (yyval.attr_info)->length = 4;
      (yyval.attr_info)->nullable = true;  
    }
#line 2156 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 47: /* nullable_spec: NULL_T  */
#line 455 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                        { (yyval.number) = 1; }
#line 2162 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 48: /* nullable_spec: NOT NULL_T  */
#line 456 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                        { (yyval.number) = 0; }
#line 2168 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 49: /* nullable_spec: %empty  */
#line 457 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                        { (yyval.number) = 1; }
#line 2174 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 50: /* number: NUMBER  */
#line 461 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
           {(yyval.number) = (yyvsp[0].number);}
#line 2180 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 51: /* type: INT_T  */
#line 464 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
               { (yyval.number) = static_cast<int>(AttrType::INTS); }
#line 2186 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 52: /* type: STRING_T  */
#line 465 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
               { (yyval.number) = static_cast<int>(AttrType::CHARS); }
#line 2192 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 53: /* type: FLOAT_T  */
#line 466 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
               { (yyval.number) = static_cast<int>(AttrType::FLOATS); }
#line 2198 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 54: /* type: VECTOR_T  */
#line 467 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
               { (yyval.number) = static_cast<int>(AttrType::VECTORS); }
#line 2204 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 55: /* type: DATE_T  */
#line 468 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
             { (yyval.number) = static_cast<int>(AttrType::DATES); }
#line 2210 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 56: /* primary_key: %empty  */
#line 472 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.key_list) = nullptr;
    }
#line 2218 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 57: /* primary_key: COMMA PRIMARY KEY LBRACE attr_list RBRACE  */
#line 476 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.key_list) = (yyvsp[-1].key_list);
    }
#line 2226 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 58: /* attr_list: ID  */
#line 482 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
       {
      (yyval.key_list) = new vector<string>();
      (yyval.key_list)->push_back((yyvsp[0].cstring));
    }
#line 2235 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 59: /* attr_list: ID COMMA attr_list  */
#line 486 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                         {
      if ((yyvsp[0].key_list) != nullptr) {
        (yyval.key_list) = (yyvsp[0].key_list);
      } else {
        (yyval.key_list) = new vector<string>;
      }

      (yyval.key_list)->insert((yyval.key_list)->begin(), (yyvsp[-2].cstring));
    }
#line 2249 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 60: /* insert_stmt: INSERT INTO ID VALUES LBRACE value_list RBRACE  */
#line 499 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_INSERT);
      (yyval.sql_node)->insertion.relation_name = (yyvsp[-4].cstring);
      (yyval.sql_node)->insertion.values.swap(*(yyvsp[-1].value_list));
      delete (yyvsp[-1].value_list);
    }
#line 2260 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 61: /* value_list: value  */
#line 509 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.value_list) = new vector<Value>;
      (yyval.value_list)->emplace_back(*(yyvsp[0].value));
      delete (yyvsp[0].value);
    }
#line 2270 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 62: /* value_list: value_list COMMA value  */
#line 514 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                             { 
      (yyval.value_list) = (yyvsp[-2].value_list);
      (yyval.value_list)->emplace_back(*(yyvsp[0].value));
      delete (yyvsp[0].value);
    }
#line 2280 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 63: /* value: NUMBER  */
#line 521 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
           {
      (yyval.value) = new Value((int)(yyvsp[0].number));
      (yyloc) = (yylsp[0]);
    }
#line 2289 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 64: /* value: FLOAT  */
#line 525 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
           {
      (yyval.value) = new Value((float)(yyvsp[0].floats));
      (yyloc) = (yylsp[0]);
    }
#line 2298 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 65: /* value: SSS  */
#line 529 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
         {
      char *tmp = common::substr((yyvsp[0].cstring),1,strlen((yyvsp[0].cstring))-2);
      (yyval.value) = new Value(tmp);
      free(tmp);
    }
#line 2308 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 66: /* value: NULL_T  */
#line 534 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
            {
      (yyval.value) = new Value();
      (yyval.value)->set_null();
      (yyval.value)->set_type(AttrType::UNDEFINED);  // NULL值类型标识
      (yyloc) = (yylsp[0]);
    }
#line 2319 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 67: /* storage_format: %empty  */
#line 543 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.cstring) = nullptr;
    }
#line 2327 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 68: /* storage_format: STORAGE FORMAT EQ ID  */
#line 547 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.cstring) = (yyvsp[0].cstring);
    }
#line 2335 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 69: /* delete_stmt: DELETE FROM ID where  */
#line 554 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DELETE);
      (yyval.sql_node)->deletion.relation_name = (yyvsp[-1].cstring);
      if ((yyvsp[0].condition_list) != nullptr) {
        (yyval.sql_node)->deletion.conditions.swap(*(yyvsp[0].condition_list));
        delete (yyvsp[0].condition_list);
      }
    }
#line 2348 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 70: /* update_stmt: UPDATE ID SET ID EQ expression where  */
#line 565 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_UPDATE);
      (yyval.sql_node)->update.relation_name = (yyvsp[-5].cstring);
      (yyval.sql_node)->update.attribute_name = (yyvsp[-3].cstring);
      (yyval.sql_node)->update.expression = (yyvsp[-1].expression);
      if ((yyvsp[0].condition_list) != nullptr) {
        (yyval.sql_node)->update.conditions.swap(*(yyvsp[0].condition_list));
        delete (yyvsp[0].condition_list);
      }
    }
#line 2363 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 71: /* select_stmt: SELECT expression_list FROM rel_list where group_by having  */
#line 578 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
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
#line 2395 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 72: /* select_stmt: SELECT expression_list where  */
#line 606 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SELECT);
      if ((yyvsp[-1].expression_list) != nullptr) {
        (yyval.sql_node)->selection.expressions.swap(*(yyvsp[-1].expression_list));
        delete (yyvsp[-1].expression_list);
      }
      
      if ((yyvsp[0].condition_list) != nullptr) {
        (yyval.sql_node)->selection.conditions.swap(*(yyvsp[0].condition_list));
        delete (yyvsp[0].condition_list);
      }
      // 不设置relations，表示没有FROM子句
    }
#line 2413 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 73: /* select_stmt: SELECT expression_list  */
#line 620 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SELECT);
      if ((yyvsp[0].expression_list) != nullptr) {
        (yyval.sql_node)->selection.expressions.swap(*(yyvsp[0].expression_list));
        delete (yyvsp[0].expression_list);
      }
      // 不设置relations，表示没有FROM子句
    }
#line 2426 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 74: /* calc_stmt: CALC expression_list  */
#line 631 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CALC);
      (yyval.sql_node)->calc.expressions.swap(*(yyvsp[0].expression_list));
      delete (yyvsp[0].expression_list);
    }
#line 2436 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 75: /* expression_list: expression  */
#line 640 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.expression_list) = new vector<unique_ptr<Expression>>;
      (yyval.expression_list)->emplace_back((yyvsp[0].expression));
    }
#line 2445 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 76: /* expression_list: expression COMMA expression_list  */
#line 645 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      if ((yyvsp[0].expression_list) != nullptr) {
        (yyval.expression_list) = (yyvsp[0].expression_list);
      } else {
        (yyval.expression_list) = new vector<unique_ptr<Expression>>;
      }
      (yyval.expression_list)->emplace((yyval.expression_list)->begin(), (yyvsp[-2].expression));
    }
#line 2458 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 77: /* expression: expression '+' expression  */
#line 655 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                              {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::ADD, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2466 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 78: /* expression: expression '-' expression  */
#line 658 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                                {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::SUB, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2474 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 79: /* expression: expression '*' expression  */
#line 661 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                                {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::MUL, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2482 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 80: /* expression: expression '/' expression  */
#line 664 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                                {
      printf("DEBUG: Creating DIV expression\n");
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::DIV, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2491 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 81: /* expression: LBRACE expression RBRACE  */
#line 668 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                               {
      (yyval.expression) = (yyvsp[-1].expression);
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
    }
#line 2500 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 82: /* expression: '-' expression  */
#line 672 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                                  {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::NEGATIVE, (yyvsp[0].expression), nullptr, sql_string, &(yyloc));
    }
#line 2508 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 83: /* expression: value  */
#line 675 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
            {
      (yyval.expression) = new ValueExpr(*(yyvsp[0].value));
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
      delete (yyvsp[0].value);
    }
#line 2518 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 84: /* expression: rel_attr  */
#line 680 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
               {
      RelAttrSqlNode *node = (yyvsp[0].rel_attr);
      (yyval.expression) = new UnboundFieldExpr(node->relation_name, node->attribute_name);
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
      delete (yyvsp[0].rel_attr);
    }
#line 2529 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 85: /* expression: '*'  */
#line 686 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
          {
      (yyval.expression) = new StarExpr();
    }
#line 2537 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 86: /* expression: COUNT LBRACE expression RBRACE  */
#line 689 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                                     {
      (yyval.expression) = create_aggregate_expression("count", (yyvsp[-1].expression), sql_string, &(yyloc));
    }
#line 2545 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 87: /* expression: COUNT LBRACE expression_list RBRACE  */
#line 692 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                                          {
      (yyval.expression) = create_aggregate_expression_multi("count", (yyvsp[-1].expression_list), sql_string, &(yyloc));
    }
#line 2553 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 88: /* expression: SUM LBRACE expression RBRACE  */
#line 695 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                                   {
      (yyval.expression) = create_aggregate_expression("sum", (yyvsp[-1].expression), sql_string, &(yyloc));
    }
#line 2561 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 89: /* expression: SUM LBRACE expression_list RBRACE  */
#line 698 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                                        {
      (yyval.expression) = create_aggregate_expression_multi("sum", (yyvsp[-1].expression_list), sql_string, &(yyloc));
    }
#line 2569 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 90: /* expression: AVG LBRACE expression RBRACE  */
#line 701 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                                   {
      (yyval.expression) = create_aggregate_expression("avg", (yyvsp[-1].expression), sql_string, &(yyloc));
    }
#line 2577 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 91: /* expression: AVG LBRACE expression_list RBRACE  */
#line 704 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                                        {
      (yyval.expression) = create_aggregate_expression_multi("avg", (yyvsp[-1].expression_list), sql_string, &(yyloc));
    }
#line 2585 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 92: /* expression: MAX LBRACE expression RBRACE  */
#line 707 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                                   {
      (yyval.expression) = create_aggregate_expression("max", (yyvsp[-1].expression), sql_string, &(yyloc));
    }
#line 2593 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 93: /* expression: MAX LBRACE expression_list RBRACE  */
#line 710 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                                        {
      (yyval.expression) = create_aggregate_expression_multi("max", (yyvsp[-1].expression_list), sql_string, &(yyloc));
    }
#line 2601 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 94: /* expression: MIN LBRACE expression RBRACE  */
#line 713 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                                   {
      (yyval.expression) = create_aggregate_expression("min", (yyvsp[-1].expression), sql_string, &(yyloc));
    }
#line 2609 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 95: /* expression: MIN LBRACE expression_list RBRACE  */
#line 716 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                                        {
      (yyval.expression) = create_aggregate_expression_multi("min", (yyvsp[-1].expression_list), sql_string, &(yyloc));
    }
#line 2617 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 96: /* rel_attr: ID  */
#line 722 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
       {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->attribute_name = (yyvsp[0].cstring);
    }
#line 2626 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 97: /* rel_attr: ID DOT ID  */
#line 726 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->relation_name  = (yyvsp[-2].cstring);
      (yyval.rel_attr)->attribute_name = (yyvsp[0].cstring);
    }
#line 2636 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 98: /* relation: ID  */
#line 734 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
       {
      (yyval.cstring) = (yyvsp[0].cstring);
    }
#line 2644 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 99: /* rel_list: relation  */
#line 739 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
             {
      (yyval.relation_list) = new vector<string>();
      (yyval.relation_list)->push_back((yyvsp[0].cstring));
    }
#line 2653 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 100: /* rel_list: relation COMMA rel_list  */
#line 743 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                              {
      if ((yyvsp[0].relation_list) != nullptr) {
        (yyval.relation_list) = (yyvsp[0].relation_list);
      } else {
        (yyval.relation_list) = new vector<string>;
      }

      (yyval.relation_list)->insert((yyval.relation_list)->begin(), (yyvsp[-2].cstring));
    }
#line 2667 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 101: /* where: %empty  */
#line 756 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.condition_list) = nullptr;
    }
#line 2675 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 102: /* where: WHERE condition_list  */
#line 759 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                           {
      (yyval.condition_list) = (yyvsp[0].condition_list);  
    }
#line 2683 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 103: /* having: %empty  */
#line 765 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.condition_list) = nullptr;
    }
#line 2691 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 104: /* having: HAVING condition_list  */
#line 768 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                            {
      (yyval.condition_list) = (yyvsp[0].condition_list);
    }
#line 2699 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 105: /* condition_list: %empty  */
#line 774 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.condition_list) = nullptr;
    }
#line 2707 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 106: /* condition_list: condition  */
#line 777 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                {
      (yyval.condition_list) = new vector<ConditionSqlNode>;
      (yyval.condition_list)->emplace_back(*(yyvsp[0].condition));
      delete (yyvsp[0].condition);
    }
#line 2717 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 107: /* condition_list: condition AND condition_list  */
#line 782 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
                                   {
      (yyval.condition_list) = (yyvsp[0].condition_list);
      (yyval.condition_list)->emplace_back(*(yyvsp[-2].condition));
      delete (yyvsp[-2].condition);
    }
#line 2727 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 108: /* condition: expression comp_op expression  */
#line 790 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
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
#line 2744 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 109: /* condition: expression IS NULL_T  */
#line 803 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
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
#line 2761 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 110: /* condition: expression IS NOT NULL_T  */
#line 816 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
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
#line 2778 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 111: /* condition: expression  */
#line 829 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
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
#line 2795 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 112: /* comp_op: EQ  */
#line 844 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
         { (yyval.comp) = EQUAL_TO; }
#line 2801 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 113: /* comp_op: LT  */
#line 845 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
         { (yyval.comp) = LESS_THAN; }
#line 2807 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 114: /* comp_op: GT  */
#line 846 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
         { (yyval.comp) = GREAT_THAN; }
#line 2813 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 115: /* comp_op: LE  */
#line 847 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
         { (yyval.comp) = LESS_EQUAL; }
#line 2819 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 116: /* comp_op: GE  */
#line 848 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
         { (yyval.comp) = GREAT_EQUAL; }
#line 2825 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 117: /* comp_op: NE  */
#line 849 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
         { (yyval.comp) = NOT_EQUAL; }
#line 2831 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 118: /* group_by: %empty  */
#line 855 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.expression_list) = nullptr;
    }
#line 2839 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 119: /* group_by: GROUP BY expression_list  */
#line 859 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.expression_list) = (yyvsp[0].expression_list); 
    }
#line 2847 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 120: /* load_data_stmt: LOAD DATA INFILE SSS INTO TABLE ID  */
#line 865 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      char *tmp_file_name = common::substr((yyvsp[-3].cstring), 1, strlen((yyvsp[-3].cstring)) - 2);
      
      (yyval.sql_node) = new ParsedSqlNode(SCF_LOAD_DATA);
      (yyval.sql_node)->load_data.relation_name = (yyvsp[0].cstring);
      (yyval.sql_node)->load_data.file_name = tmp_file_name;
      free(tmp_file_name);
    }
#line 2860 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 121: /* explain_stmt: EXPLAIN command_wrapper  */
#line 877 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_EXPLAIN);
      (yyval.sql_node)->explain.sql_node = unique_ptr<ParsedSqlNode>((yyvsp[0].sql_node));
    }
#line 2869 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;

  case 122: /* set_variable_stmt: SET ID EQ value  */
#line 885 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SET_VARIABLE);
      (yyval.sql_node)->set_variable.name  = (yyvsp[-2].cstring);
      (yyval.sql_node)->set_variable.value = *(yyvsp[0].value);
      delete (yyvsp[0].value);
    }
#line 2880 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"
    break;


#line 2884 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.cpp"

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

#line 896 "/home/chris/OBZen-zhanghao-2024/src/observer/sql/parser/yacc_sql.y"

//_____________________________________________________________________
extern void scan_string(const char *str, yyscan_t scanner);

int sql_parse(const char *s, ParsedSqlResult *sql_result) {
  yyscan_t scanner;
  std::vector<char *> allocated_strings;
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

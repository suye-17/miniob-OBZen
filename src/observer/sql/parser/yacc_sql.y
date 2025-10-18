
%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/log/log.h"
#include "common/lang/string.h"
#include "common/types.h"
#include "sql/parser/parse_defs.h"
#include "sql/parser/yacc_sql.hpp"
#include "sql/parser/lex_sql.h"
#include "sql/expr/expression.h"
#include "observer/common/utils.h"

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

DistanceFunctionExpr *create_distance_function_expression(DistanceFunctionExpr::Type type,
                                                          Expression *left,
                                                          Expression *right,
                                                          const char *sql_string,
                                                          YYLTYPE *llocp)
{
  DistanceFunctionExpr *expr = new DistanceFunctionExpr(type, unique_ptr<Expression>(left), unique_ptr<Expression>(right));
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

%}

%define api.pure full
%define parse.error verbose
/** 启用位置标识 **/
%locations
%lex-param { yyscan_t scanner }
/** 这些定义了在yyparse函数中的参数 **/
%parse-param { const char * sql_string }
%parse-param { ParsedSqlResult * sql_result }
%parse-param { void * scanner }

//标识tokens
%token  SEMICOLON
        BY
        ORDER
        ASC
        CREATE
        DROP
        GROUP
        HAVING
        TABLE
        TABLES
        INDEX
        UNIQUE
        CALC
        SELECT
        DESC
        SHOW
        SYNC
        INSERT
        DELETE
        UPDATE
        LBRACE
        RBRACE
        LSBRACE
        RSBRACE
        COMMA
        TRX_BEGIN
        TRX_COMMIT
        TRX_ROLLBACK
        INT_T
        STRING_T
        FLOAT_T
        DATE_T
        NULL_T
        NOT
        IS
        VECTOR_T
        TEXT_T
        HELP
        EXIT
        DOT //QUOTE
        INTO
        VALUES
        FROM
        WHERE
        AND
        SET
        ON
        LOAD
        DATA
        INFILE
        EXPLAIN
        STORAGE
        FORMAT
        PRIMARY
        KEY
        ANALYZE
        EQ
        LT
        GT
        LE
        GE
        NE
        L2_DISTANCE
        COSINE_DISTANCE
        INNER_PRODUCT
        COUNT
        SUM
        AVG
        MAX
        MIN
        IN
        LIKE
        EXISTS
        INNER
        JOIN

/** union 中定义各种数据类型，真实生成的代码也是union类型，所以不能有非POD类型的数据 **/
%union {
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
}

%destructor { delete $$; } <condition>
%destructor { delete $$; } <value>
%destructor { delete $$; } <rel_attr>
%destructor { delete $$; } <attr_infos>
%destructor { delete $$; } <attr_info>
%destructor { delete $$; } <expression>
%destructor { delete $$; } <expression_list>
%destructor { delete $$; } <value_list>
%destructor { delete $$; } <condition_list>
// %destructor { delete $$; } <rel_attr_list>
%destructor { delete $$; } <relation_list>
%destructor { delete $$; } <key_list>
%destructor { delete $$; } <update_list>

%token <number> NUMBER
%token <floats> FLOAT
%token <cstring> ID
%token <cstring> SSS
%token <cstring> VECTOR_LITERAL
//非终结符

/** type 定义了各种解析后的结果输出的是什么类型。类型对应了 union 中的定义的成员变量名称 **/
%type <number>              type
%type <condition>           condition
%type <value>               value
%type <number>              number
%type <cstring>             relation
%type <comp>                comp_op
%type <rel_attr>            rel_attr
%type <attr_infos>          attr_def_list
%type <attr_info>           attr_def
%type <number>              nullable_spec
%type <value_list>          value_list
%type <condition_list>      where
%type <condition_list>      having
%type <condition_list>      condition_list
%type <condition_list>      on_conditions
%type <cstring>             storage_format
%type <key_list>            primary_key
%type <key_list>            attr_list
%type <key_list>            attribute_name_list
%type <relation_list>       rel_list
%type <join_list>           join_list
%type <expression>          expression
%type <expression_list>     expression_list
%type <expression_list>     group_by
%type <update_list>         update_list
%type <sql_node>            calc_stmt
%type <sql_node>            select_stmt
%type <sql_node>            insert_stmt
%type <sql_node>            update_stmt
%type <sql_node>            delete_stmt
%type <sql_node>            create_table_stmt
%type <sql_node>            drop_table_stmt
%type <sql_node>            analyze_table_stmt
%type <sql_node>            show_tables_stmt
%type <sql_node>            desc_table_stmt
%type <sql_node>            create_index_stmt
%type <sql_node>            show_index_stmt
%type <sql_node>            drop_index_stmt
%type <sql_node>            sync_stmt
%type <sql_node>            begin_stmt
%type <sql_node>            commit_stmt
%type <sql_node>            rollback_stmt
%type <sql_node>            load_data_stmt
%type <sql_node>            explain_stmt
%type <sql_node>            set_variable_stmt
%type <sql_node>            help_stmt
%type <sql_node>            exit_stmt
%type <sql_node>            command_wrapper
// commands should be a list but I use a single command instead
%type <sql_node>            commands

%left '+' '-'
%left '*' '/'
%left EQ NE LT LE GT GE
%right UMINUS
%%

commands: command_wrapper opt_semicolon  //commands or sqls. parser starts here.
  {
    unique_ptr<ParsedSqlNode> sql_node = unique_ptr<ParsedSqlNode>($1);
    sql_result->add_sql_node(std::move(sql_node));
  }
  ;

command_wrapper:
    calc_stmt
  | select_stmt
  | insert_stmt
  | update_stmt
  | delete_stmt
  | create_table_stmt
  | drop_table_stmt
  | analyze_table_stmt
  | show_tables_stmt
  | show_index_stmt
  | desc_table_stmt
  | create_index_stmt
  | drop_index_stmt
  | sync_stmt
  | begin_stmt
  | commit_stmt
  | rollback_stmt
  | load_data_stmt
  | explain_stmt
  | set_variable_stmt
  | help_stmt
  | exit_stmt
    ;

exit_stmt:      
    EXIT {
      (void)yynerrs;  // 这么写为了消除yynerrs未使用的告警。如果你有更好的方法欢迎提PR
      $$ = new ParsedSqlNode(SCF_EXIT);
    };

help_stmt:
    HELP {
      $$ = new ParsedSqlNode(SCF_HELP);
    };

sync_stmt:
    SYNC {
      $$ = new ParsedSqlNode(SCF_SYNC);
    }
    ;

begin_stmt:
    TRX_BEGIN  {
      $$ = new ParsedSqlNode(SCF_BEGIN);
    }
    ;

commit_stmt:
    TRX_COMMIT {
      $$ = new ParsedSqlNode(SCF_COMMIT);
    }
    ;

rollback_stmt:
    TRX_ROLLBACK  {
      $$ = new ParsedSqlNode(SCF_ROLLBACK);
    }
    ;

drop_table_stmt:    /*drop table 语句的语法解析树*/
    DROP TABLE ID {
      $$ = new ParsedSqlNode(SCF_DROP_TABLE);
      $$->drop_table.relation_name = $3;
    };

analyze_table_stmt:  /* analyze table 语法的语法解析树*/
    ANALYZE TABLE ID {
      $$ = new ParsedSqlNode(SCF_ANALYZE_TABLE);
      $$->analyze_table.relation_name = $3;
    }
    ;

show_tables_stmt:
    SHOW TABLES {
      $$ = new ParsedSqlNode(SCF_SHOW_TABLES);
    }
    ;

desc_table_stmt:
    DESC ID  {
      $$ = new ParsedSqlNode(SCF_DESC_TABLE);
      $$->desc_table.relation_name = $2;
    }
    ;

create_index_stmt:    /*create index 语句的语法解析树*/
    CREATE INDEX ID ON ID LBRACE attribute_name_list RBRACE
    {
      $$ = new ParsedSqlNode(SCF_CREATE_INDEX);
      CreateIndexSqlNode &create_index = $$->create_index;
      create_index.index_name = $3;
      create_index.relation_name = $5;
      create_index.is_unique = false;
      create_index.attribute_names = std::move (*$7);
      delete $7;
    }
    | CREATE UNIQUE INDEX ID ON ID LBRACE attribute_name_list RBRACE
    {
      $$ = new ParsedSqlNode(SCF_CREATE_INDEX);
      CreateIndexSqlNode &create_index = $$->create_index;
      create_index.index_name = $4;
      create_index.relation_name = $6;
      create_index.is_unique = true;
      create_index.attribute_names = std::move (*$8);
      delete $8;
    }
    ;
attribute_name_list:
    ID
    {
      $$ = new vector<string> ();
      $$->push_back($1);
    }
    | attribute_name_list COMMA ID
    {
      $$ = $1;
      $$->push_back($3);
    }
    ;
drop_index_stmt:      /*drop index 语句的语法解析树*/
    DROP INDEX ID ON ID
    {
      $$ = new ParsedSqlNode(SCF_DROP_INDEX);
      $$->drop_index.index_name = $3;
      $$->drop_index.relation_name = $5;
    }
    ;
show_index_stmt:     /*显示表中的索引*/
    SHOW INDEX FROM ID 
    {
      $$ = new ParsedSqlNode(SCF_SHOW_INDEX);
      $$->show_index.relation_name = $4;  
    } 
    ;
create_table_stmt:    /*create table 语句的语法解析树*/
    CREATE TABLE ID LBRACE attr_def_list primary_key RBRACE storage_format
    {
      $$ = new ParsedSqlNode(SCF_CREATE_TABLE);
      CreateTableSqlNode &create_table = $$->create_table;
      create_table.relation_name = $3;
      //free($3);

      create_table.attr_infos.swap(*$5);
      delete $5;

      if ($6 != nullptr) {
        create_table.primary_keys.swap(*$6);
        delete $6;
      }
      if ($8 != nullptr) {
        create_table.storage_format = $8;
      }
    }
    ;
    
attr_def_list:
    attr_def
    {
      $$ = new vector<AttrInfoSqlNode>;
      $$->emplace_back(*$1);
      delete $1;
    }
    | attr_def_list COMMA attr_def
    {
      $$ = $1;
      $$->emplace_back(*$3);
      delete $3;
    }
    ;
    
attr_def:
    ID type LBRACE number RBRACE nullable_spec
    {
      $$ = new AttrInfoSqlNode;
      $$->type = (AttrType)$2;
      $$->name = $1;
      if ($$->type == AttrType::VECTORS) {
        $$->length =  $4 * sizeof(float);
      } else {
        $$->length = $4;
      }
      $$->nullable = ($6 == 1);
    }
    | ID type nullable_spec
    {
      $$ = new AttrInfoSqlNode;
      $$->type = (AttrType)$2;
      $$->name = $1;
      if ($$->type == AttrType::TEXTS) {
        $$->length = 784;    // TEXT字段在记录中占用: 16字节指针 + 768字节内联数据
      } else {
        $$->length = 4;
      }
      $$->nullable = ($3 == 1);
    }
    | ID type LBRACE number RBRACE 
    {
      $$ = new AttrInfoSqlNode;
      $$->type = (AttrType)$2;
      $$->name = $1;
      if ($$->type == AttrType::VECTORS) {
        $$->length =  $4 * sizeof(float);
      } else {
        $$->length = $4;
      }
      $$->nullable = true;  
    }
    | ID type
    {
      $$ = new AttrInfoSqlNode;
      $$->type = (AttrType)$2;
      $$->name = $1;
      if ($$->type == AttrType::TEXTS) {
        $$->length = 784;    // TEXT字段在记录中占用: 16字节指针 + 768字节内联数据
      } else {
        $$->length = 4;
      }
      $$->nullable = true;  
    }
    ;

nullable_spec:
    NOT NULL_T          { $$ = 0; }      // nullable = false  
    | /* empty */       { $$ = 1; }      // 默认为nullable = true
    ;

number:
    NUMBER {$$ = $1;}
    ;
type:
    INT_T      { $$ = static_cast<int>(AttrType::INTS); }
    | STRING_T { $$ = static_cast<int>(AttrType::CHARS); }
    | FLOAT_T  { $$ = static_cast<int>(AttrType::FLOATS); }
    | DATE_T { $$ = static_cast<int>(AttrType::DATES); }
    | VECTOR_T { $$ = static_cast<int>(AttrType::VECTORS); }
    | TEXT_T { $$ = static_cast<int>(AttrType::TEXTS); }
    ;
primary_key:
    /* empty */
    {
      $$ = nullptr;
    }
    | COMMA PRIMARY KEY LBRACE attr_list RBRACE
    {
      $$ = $5;
    }
    ;

attr_list:
    ID {
      $$ = new vector<string>();
      $$->push_back($1);
    }
    | ID COMMA attr_list {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new vector<string>;
      }

      $$->insert($$->begin(), $1);
    }
    ;

insert_stmt:        /*insert   语句的语法解析树*/
    INSERT INTO ID VALUES LBRACE value_list RBRACE 
    {
      $$ = new ParsedSqlNode(SCF_INSERT);
      $$->insertion.relation_name = $3;
      $$->insertion.values.swap(*$6);
      delete $6;
    }
    ;

value_list:
    value
    {
      $$ = new vector<Value>;
      $$->emplace_back(*$1);
      delete $1;
    }
    | value_list COMMA value { 
      $$ = $1;
      $$->emplace_back(*$3);
      delete $3;
    }
    ;
value:
    NUMBER {
      $$ = new Value((int)$1);
      @$ = @1;
    }
    |FLOAT {
      $$ = new Value((float)$1);
      @$ = @1;
    }
    |SSS {
      char *tmp = common::substr($1,1,strlen($1)-2);
      size_t str_len = strlen(tmp);
      
      // 注意：这里不再在解析阶段限制字符串长度
      // 长度检查推迟到类型转换阶段（cast_to TEXTS）和插入阶段处理
      // 这样可以支持不同字段类型的不同长度限制，并提供更准确的错误信息
      $$ = new Value(tmp, str_len);
      free(tmp);
    }
    |NULL_T {
      $$ = new Value();
      $$->set_null();
      $$->set_type(AttrType::UNDEFINED);  // NULL值类型标识
      @$ = @1;
    }
    |VECTOR_LITERAL {
       std::vector<float> elements;
       RC rc = parse_vector_literal($1, elements);
       if (rc != RC::SUCCESS) {
         LOG_WARN("Failed to parse vector literal: %s", $1);
         YYABORT;  // 语法解析错误
       }
       $$ = new Value();
       $$->set_vector(elements);
       @$ = @1;
    }
    ;
storage_format:
    /* empty */
    {
      $$ = nullptr;
    }
    | STORAGE FORMAT EQ ID
    {
      $$ = $4;
    }
    ;
    
delete_stmt:    /*  delete 语句的语法解析树*/
    DELETE FROM ID where 
    {
      $$ = new ParsedSqlNode(SCF_DELETE);
      $$->deletion.relation_name = $3;
      if ($4 != nullptr) {
        $$->deletion.conditions.swap(*$4);
        delete $4;
      }
    }
    ;
update_stmt:      /*  update 语句的语法解析树*/
    UPDATE ID SET update_list where 
    {
      $$ = new ParsedSqlNode(SCF_UPDATE);
      $$->update.relation_name = $2;
      $$->update.attribute_names.swap($4->attribute_names);
      $$->update.expressions.swap($4->expressions);
      if ($5 != nullptr) {
        $$->update.conditions.swap(*$5);
        delete $5;
      }
      delete $4;
      // 不需要 free($2)，sql_parse 会统一清理 allocated_strings
    }
    ;
    
update_list:
    ID EQ expression
    {
      $$ = new UpdateList();
      $$->attribute_names.push_back($1);
      $$->expressions.push_back($3);
      // 不需要 free($1)，sql_parse 会统一清理 allocated_strings
    }
    | update_list COMMA ID EQ expression
    {
      $$ = $1;
      $$->attribute_names.push_back($3);
      $$->expressions.push_back($5);
      // 不需要 free($3)，sql_parse 会统一清理 allocated_strings
    }
    ;

select_stmt:        /*  select 语句的语法解析树*/
    SELECT expression_list FROM rel_list join_list where group_by having
    {
      $$ = new ParsedSqlNode(SCF_SELECT);
      if ($2 != nullptr) {
        $$->selection.expressions.swap(*$2);
        delete $2;
      }

      if ($4 != nullptr) {
        $$->selection.relations.swap(*$4);
        delete $4;
      }

      if ($5 != nullptr) {
        $$->selection.joins.swap(*$5);
        delete $5;
      }

      if ($6 != nullptr) {
        $$->selection.conditions.swap(*$6);
        delete $6;
      }

      if ($7 != nullptr) {
        $$->selection.group_by.swap(*$7);
        delete $7;
      }

      if ($8 != nullptr) {
        $$->selection.having.swap(*$8);
        delete $8;
      }
    }
    | SELECT expression_list WHERE condition_list  /* 不带FROM子句但有WHERE的SELECT语句 */
    {
      $$ = new ParsedSqlNode(SCF_SELECT);
      if ($2 != nullptr) {
        $$->selection.expressions.swap(*$2);
        delete $2;
      }
      
      if ($4 != nullptr) {
        $$->selection.conditions.swap(*$4);
        delete $4;
      }
      // 不设置relations，表示没有FROM子句
    }
    | SELECT expression_list  /* 不带FROM和WHERE子句的SELECT语句 */
    {
      $$ = new ParsedSqlNode(SCF_SELECT);
      if ($2 != nullptr) {
        $$->selection.expressions.swap(*$2);
        delete $2;
      }
      // 不设置relations，表示没有FROM子句
    }
    ;
calc_stmt:
    CALC expression_list
    {
      $$ = new ParsedSqlNode(SCF_CALC);
      $$->calc.expressions.swap(*$2);
      delete $2;
    }
    ;

expression_list:
    expression
    {
      $$ = new vector<unique_ptr<Expression>>;
      $$->emplace_back($1);
    }
    | expression COMMA expression_list
    {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new vector<unique_ptr<Expression>>;
      }
      $$->emplace($$->begin(), $1);
    }
    ;
expression:
    expression '+' expression {
      $$ = create_arithmetic_expression(ArithmeticExpr::Type::ADD, $1, $3, sql_string, &@$);
    }
    | expression '-' expression {
      $$ = create_arithmetic_expression(ArithmeticExpr::Type::SUB, $1, $3, sql_string, &@$);
    }
    | expression '*' expression {
      $$ = create_arithmetic_expression(ArithmeticExpr::Type::MUL, $1, $3, sql_string, &@$);
    }
    | expression '/' expression {
      printf("DEBUG: Creating DIV expression\n");
      $$ = create_arithmetic_expression(ArithmeticExpr::Type::DIV, $1, $3, sql_string, &@$);
    }
    | LBRACE expression RBRACE {
      $$ = $2;
      $$->set_name(token_name(sql_string, &@$));
    }
    | '-' expression %prec UMINUS {
      $$ = create_arithmetic_expression(ArithmeticExpr::Type::NEGATIVE, $2, nullptr, sql_string, &@$);
    }
    | value {
      $$ = new ValueExpr(*$1);
      $$->set_name(token_name(sql_string, &@$));
      delete $1;
    }
    | rel_attr {
      RelAttrSqlNode *node = $1;
      $$ = new UnboundFieldExpr(node->relation_name, node->attribute_name);
      $$->set_name(token_name(sql_string, &@$));
      delete $1;
    }
    | '*' {
      $$ = new StarExpr();
    }
    | COUNT LBRACE '*' RBRACE {
      $$ = create_aggregate_expression("count", new StarExpr(), sql_string, &@$);
    }
    | COUNT LBRACE expression RBRACE {
      $$ = create_aggregate_expression("count", $3, sql_string, &@$);
    }
    | SUM LBRACE expression RBRACE {
      $$ = create_aggregate_expression("sum", $3, sql_string, &@$);
    }
    | AVG LBRACE expression RBRACE {
      $$ = create_aggregate_expression("avg", $3, sql_string, &@$);
    }
    | MAX LBRACE expression RBRACE {
      $$ = create_aggregate_expression("max", $3, sql_string, &@$);
    }
    | MIN LBRACE expression RBRACE {
      $$ = create_aggregate_expression("min", $3, sql_string, &@$);
    }
    | LBRACE select_stmt RBRACE {
      // 子查询表达式
      $$ = new SubqueryExpr(SelectSqlNode::create_copy(&($2->selection)));
      $$->set_name(token_name(sql_string, &@$));
      delete $2;
    }
    | expression IN LBRACE expression_list RBRACE {
      // IN (value_list) 表达式
      vector<unique_ptr<Expression>> value_list;
      if ($4 != nullptr) {
        value_list = std::move(*$4);
        delete $4;
      }
      $$ = new InExpr(false, unique_ptr<Expression>($1), std::move(value_list));
      $$->set_name(token_name(sql_string, &@$));
    }
    | expression NOT IN LBRACE expression_list RBRACE {
      // NOT IN (value_list) 表达式
      vector<unique_ptr<Expression>> value_list;
      if ($5 != nullptr) {
        value_list = std::move(*$5);
        delete $5;
      }
      $$ = new InExpr(true, unique_ptr<Expression>($1), std::move(value_list));
      $$->set_name(token_name(sql_string, &@$));
    }
    | expression IN LBRACE select_stmt RBRACE {
      // IN (SELECT ...) 表达式
      auto subquery = new SubqueryExpr(SelectSqlNode::create_copy(&($4->selection)));
      $$ = new InExpr(false, unique_ptr<Expression>($1), unique_ptr<Expression>(subquery));
      $$->set_name(token_name(sql_string, &@$));
      delete $4;
    }
    | expression NOT IN LBRACE select_stmt RBRACE {
      // NOT IN (SELECT ...) 表达式
      auto subquery = new SubqueryExpr(SelectSqlNode::create_copy(&($5->selection)));
      $$ = new InExpr(true, unique_ptr<Expression>($1), unique_ptr<Expression>(subquery));
      $$->set_name(token_name(sql_string, &@$));
      delete $5;
    }
    | EXISTS LBRACE select_stmt RBRACE {
      // EXISTS (SELECT ...) 表达式
      auto subquery = new SubqueryExpr(SelectSqlNode::create_copy(&($3->selection)));
      $$ = new ExistsExpr(false, unique_ptr<Expression>(subquery));
      $$->set_name(token_name(sql_string, &@$));
      delete $3;
    }
    | NOT EXISTS LBRACE select_stmt RBRACE {
      // NOT EXISTS (SELECT ...) 表达式
      auto subquery = new SubqueryExpr(SelectSqlNode::create_copy(&($4->selection)));
      $$ = new ExistsExpr(true, unique_ptr<Expression>(subquery));
      $$->set_name(token_name(sql_string, &@$));
      delete $4;
    }
    | L2_DISTANCE LBRACE expression COMMA expression RBRACE {
      $$ = create_distance_function_expression(DistanceFunctionExpr::Type::L2_DISTANCE, $3, $5, sql_string, &@$);
    }
    | COSINE_DISTANCE LBRACE expression COMMA expression RBRACE {
      $$ = create_distance_function_expression(DistanceFunctionExpr::Type::COSINE_DISTANCE, $3, $5, sql_string, &@$);
    }
    | INNER_PRODUCT LBRACE expression COMMA expression RBRACE {
      $$ = create_distance_function_expression(DistanceFunctionExpr::Type::INNER_PRODUCT, $3, $5, sql_string, &@$);
    }
    ;

rel_attr:
    ID {
      $$ = new RelAttrSqlNode;
      $$->attribute_name = $1;
    }
    | ID DOT ID {
      $$ = new RelAttrSqlNode;
      $$->relation_name  = $1;
      $$->attribute_name = $3;
    }
    ;

relation:
    ID {
      $$ = $1;
    }
    ;
rel_list:
    relation {
      $$ = new vector<string>();
      $$->push_back($1);
    }
    | relation COMMA rel_list {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new vector<string>;
      }

      $$->insert($$->begin(), $1);
    }
    ;

where:
    /* empty */
    {
      $$ = nullptr;
    }
    | WHERE condition_list {
      $$ = $2;  
    }
    ;
having:
    /* empty */
    {
      $$ = nullptr;
    }
    | HAVING condition_list {
      $$ = $2;
    }
    ;
condition_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | condition {
      $$ = new vector<ConditionSqlNode>;
      $$->push_back(*$1);
      delete $1;
    }
    | condition AND condition_list {
      if ($3 == nullptr) {
        $$ = new vector<ConditionSqlNode>;
      } else {
        $$ = $3;
      }
      $$->insert($$->begin(), *$1);
      delete $1;
    }
    ;
condition:
    expression comp_op expression 
    {
      printf("DEBUG: unified condition expression comp_op expression\n");
      $$ = new ConditionSqlNode;
      $$->comp = $2;
      $$->left_expression = $1;
      $$->right_expression = $3;
      $$->is_expression_condition = true;
      
      // 清零旧字段以确保一致性
      $$->left_is_attr = 0;
      $$->right_is_attr = 0;
    }
    | expression IS NULL_T
    {
      printf("DEBUG: IS NULL condition\n");
      $$ = new ConditionSqlNode;
      $$->comp = IS_NULL;
      $$->left_expression = $1;
      $$->right_expression = nullptr;
      $$->is_expression_condition = true;
      
      // 清零旧字段以确保一致性
      $$->left_is_attr = 0;
      $$->right_is_attr = 0;
    }
    | expression IS NOT NULL_T
    {
      printf("DEBUG: IS NOT NULL condition\n");
      $$ = new ConditionSqlNode;
      $$->comp = IS_NOT_NULL;
      $$->left_expression = $1;
      $$->right_expression = nullptr;
      $$->is_expression_condition = true;
      
      // 清零旧字段以确保一致性
      $$->left_is_attr = 0;
      $$->right_is_attr = 0;
    }
    | expression
    {
      printf("DEBUG: single expression condition\n");
      $$ = new ConditionSqlNode;
      $$->comp = NO_OP;  // 标识单独表达式条件
      $$->left_expression = $1;
      $$->right_expression = nullptr;
      $$->is_expression_condition = true;
      
      // 清零旧字段以确保一致性
      $$->left_is_attr = 0;
      $$->right_is_attr = 0;
    }
    ;

comp_op:
      EQ { $$ = EQUAL_TO; }
    | LT { $$ = LESS_THAN; }
    | GT { $$ = GREAT_THAN; }
    | LE { $$ = LESS_EQUAL; }
    | GE { $$ = GREAT_EQUAL; }
    | NE { $$ = NOT_EQUAL; }
    | LIKE { $$ = LIKE_OP; }  // 新增LIKE操作符
    | NOT LIKE { $$ = NOT_LIKE_OP; }  // 新增NOT LIKE操作符
    ;

// ON条件列表（用于JOIN ON子句，支持AND连接多个条件）
on_conditions:
    expression comp_op expression {
      $$ = new vector<ConditionSqlNode>;
      ConditionSqlNode *cond = new ConditionSqlNode;
      cond->comp = $2;
      cond->left_expression = $1;
      cond->right_expression = $3;
      cond->is_expression_condition = true;
      cond->left_is_attr = 0;
      cond->right_is_attr = 0;
      $$->push_back(*cond);
      delete cond;
    }
    | expression comp_op expression AND on_conditions {
      if ($5 == nullptr) {
        $$ = new vector<ConditionSqlNode>;
      } else {
        $$ = $5;
      }
      ConditionSqlNode cond;
      cond.comp = $2;
      cond.left_expression = $1;
      cond.right_expression = $3;
      cond.is_expression_condition = true;
      cond.left_is_attr = 0;
      cond.right_is_attr = 0;
      $$->insert($$->begin(), cond);
    }
    ;

// JOIN functionality - unified join_list approach
join_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | INNER JOIN relation ON on_conditions
    {
      $$ = new vector<JoinSqlNode>;
      JoinSqlNode join_node;
      join_node.type = JoinType::INNER_JOIN;
      join_node.relation = $3;
      
      // 复制所有ON条件
      if ($5 != nullptr) {
        join_node.conditions = *$5;
        delete $5;
      }
      
      $$->push_back(join_node);
    }
    | join_list INNER JOIN relation ON on_conditions
    {
      if ($1 != nullptr) {
        $$ = $1;
      } else {
        $$ = new vector<JoinSqlNode>;
      }
      
      JoinSqlNode join_node;
      join_node.type = JoinType::INNER_JOIN;
      join_node.relation = $4;
      
      // 复制所有ON条件
      if ($6 != nullptr) {
        join_node.conditions = *$6;
        delete $6;
      }
      
      $$->push_back(join_node);
    }
    ;

group_by:
    /* empty */
    {
      $$ = nullptr;
    }
    | GROUP BY expression_list
    {
      $$ = $3; 
    }
    ;
load_data_stmt:
    LOAD DATA INFILE SSS INTO TABLE ID 
    {
      char *tmp_file_name = common::substr($4, 1, strlen($4) - 2);
      
      $$ = new ParsedSqlNode(SCF_LOAD_DATA);
      $$->load_data.relation_name = $7;
      $$->load_data.file_name = tmp_file_name;
      free(tmp_file_name);
    }
    ;

explain_stmt:
    EXPLAIN command_wrapper
    {
      $$ = new ParsedSqlNode(SCF_EXPLAIN);
      $$->explain.sql_node = unique_ptr<ParsedSqlNode>($2);
    }
    ;

set_variable_stmt:
    SET ID EQ value
    {
      $$ = new ParsedSqlNode(SCF_SET_VARIABLE);
      $$->set_variable.name  = $2;
      $$->set_variable.value = *$4;
      delete $4;
    }
    ;

opt_semicolon: /*empty*/
    | SEMICOLON
    ;
%%
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
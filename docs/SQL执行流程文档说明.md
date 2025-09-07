# SQL执行流程文档添加说明

flowchart TD
    A["用户输入 SQL 字符串<br/>SELECT * FROM users WHERE age > 25"] --> B[词法分析 lex_sql.l]
    
    B --> C[语法分析 yacc_sql.y]
    C --> D["生成语法树<br/>ParsedSqlNode"]
    
    D --> E[语义解析 Resolver]
    E --> F["转换为内部结构<br/>Statement"]
    
    F --> G[查询优化 Optimizer]
    G --> H{"SQL类型判断"}
    
    H -->|查询类型| I[生成逻辑算子<br/>LogicalOperator]
    H -->|命令类型| J[生成命令执行器<br/>CommandExecutor]
    
    I --> K[优化重写]
    K --> L["生成物理算子<br/>PhysicalOperator"]
    
    L --> M[执行引擎 Executor]
    J --> M
    
    M --> N[火山模型执行]
    N --> O["调用 next() 方法<br/>获取每行结果"]
    
    O --> P[访问存储引擎]
    P --> Q[Buffer Pool]
    P --> R[B+ Tree 索引]
    P --> S[Record Manager]
    
    Q --> T[返回结果集 SqlResult]
    R --> T
    S --> T
    
    T --> U[返回给客户端]

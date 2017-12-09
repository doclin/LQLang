# LQ Language(parser part)
by whu Doclin


## ~LL(1) Grammer
```
<program>           ::= <statements>
<statements>        ::= <stmt> <stmts>
<stmts>             ::= <statements> | e
<stmt>              ::= <varStmt> | <ifStmt> | <whileStmt> | <breakStmt> 
                    |   <assignStmt> | <funcStmt> | <callStmt> | <retStmt>
<varStmt>           ::= <type> <varList> ;
<type>              ::= <basicType> <arrType>
<basicType>         ::= int | double 
<arrType>           ::= [UNSIGNEDINT] | e
<varList>           ::= ID <vars>
<vars>              ::= , ID <vars> | e
<ifStmt>            ::= if (<expr>) {<statements>} <elseStmt>
<elseStmt>          ::= else {<statements>} | e
<whileStmt>         ::= while (<expr>) {<statements>}
<breakStmt>         ::= break ;
<assignStmt>        ::= <value> = <expr>;
<value>             ::= ID <arrType>
<funcStmt>          ::= <funcType> ID (<paramList>) {<statements>}
<funcType>          ::= <type> | void
<paramList>         ::= <type> ID <params> | e
<params>            ::= , <type> ID <params> | e
<callStmt>          ::= ID(<argList>);
<argList>           ::= <expr> <args> | e
<args>              ::= , <expr> <args> | e
<retStmt>           ::= return <expr>;

<expr>              ::= <expr1> <term>
<term>              ::= <level1> <expr1> | e
<level1>            ::= < | > | == | <= | >= | !=
<expr1>             ::= <expr2> <term1>
<term1>             ::= <level2> <expr2> <term1> | e
<level2>            ::= + | -
<expr2>             ::= <expr3> <term2>
<term2>             ::= <level3> <expr3> <term2> | e
<level3>            ::= * | / 
<expr3>             ::= <level4> <expr4>
<level4>            ::= - | e
<expr4>             ::= (<expr>) | <value> | ID(<argList>) | INT | DOUBLE 
```

采用C-style作用域
语法最后改



数组为引用类型，使用iarray,darray分配

全局符号表包括全局数据和函数入口地址

栈顶esp，栈基ebp
函数调用先压

return
移动esp
返回值

指令粒度

解决回溯语法
拿纸画
构建AST，确定权责

parser简单对象
符号表封装类，单例模式
IR封装类

参考龙书：
语法树
符号表
寄存器分配

TODO：
语法树
解释器IR
解释执行IR
并行化     1day
LLVM      1day


名字查找
非法语句：
break
return
类型检查





iload       index
dload       index
istore      index
dstore      index
istoren     int
dstoren     double
iconst      int
dconst      double
iarray      size
darray      size
if          code
jump        code

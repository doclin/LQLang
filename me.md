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


类型检查：
函数调用参数
expr类型
void加return
内置函数




析构
异常
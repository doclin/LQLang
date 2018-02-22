# LQLang
by whu Doclin

### 项目概述
LQLang是参照C语言语法风格设计的编程语言。支持全局变量的声明，函数定义，局部变量的声明，表达式赋值，递归函数调用，if/else/while控制流转移等功能，a.lq示例了部分基本语法。LQLang解释器利用栈式虚拟机和少量特用寄存器实现了LQLang指令集的解释执行。

### 项目结构
```
├── a.lq
├── AST.cpp
├── AST.h
├── interpreter.cpp
├── interpreter.h
├── main.cpp
├── parser.cpp
├── parser.h
├── readme.md
├── symbolTable.cpp
└── symbolTable.h
```

### 项目细节
main.cpp作为解释器入口函数，读取源文件并存储字符串便于下一步处理。  
parser.cpp定义Parser类，在源文件的一趟读取中完成词法分析和递归下降语法分析，其中使用了大量内联函数优化字符处理。在语法分析过程中调用AST实例的构造接口构造抽象语法树。  
AST.cpp定义了抽象语法树AST类以及树节点ASTNode基类和不同类型子节点，同时提供外部接口用于构建语法树，并在构建过程中完成语义分析检查。AST的数据结构为自定义的树形异质链表，利用基类指针实现不同类型节点的串联，基类节点定义与If语句节点定义如下，其他类型节点见AST.h
```
class ASTNode
{
public:
    enum { VARNODE=1, FUNCNODE, CALLNODE, ASSIGNNODE, 
    IFNODE, WHILENODE, BREAKNODE, RETURNNODE, EXPRNODE,
     INTNODE, DOUBLENODE, VARVALNODE, OPERATORNODE };
    enum { TVOID=0, TINT, TDOUBLE, TINTARR, 
    TDOUBLEARR, TFUNCVOID, TFUNCINT, TFUNCDOUBLE };
    int nodeType;
    ASTNode* next;
    ASTNode() {}
    ASTNode(int t) : nodeType(t), next(NULL) {}
    virtual ~ASTNode() {}
};
class IfNode : public ASTNode
{
public:
    ASTNode* value;
    ASTNode* trueStmts;
    ASTNode* falseStmts;
    IfNode() : ASTNode::ASTNode(IFNODE), 
        value(NULL), trueStmts(NULL), falseStmts(NULL) {}
    virtual ~IfNode() {}
};
```
symbolTable.cpp定义了支持嵌套作用域的符号表，用于AST构建中函数及其他嵌套语句块的命名空间查找，其核心数据结构为标准库中的vector和map，数据定义如下
```
std::vector< 
    std::map<std::string, 
        std::pair<size_t, int> > > table
```
当查找名称时会从当前作用域开始向上一层逐层查找，插入名称会检查当前作用域后插入。  
interpreter.cpp定义了Interpreter类，用于将合法AST转换为线性指令，并解释执行线性指令。Interpreter定义了50种指令，参考了Java字节码以及x86汇编指令。并利用c++union类型存储中间IR
```
enum { PUSHEBP, NEWEBP, MALLOC, MALLOCARR, DTOI, ITOD,
IGSTORE, ILSTORE, DGSTORE, DLSTORE, IFCMP, IFNCMP, JUMP,
JUMPBACK, IRETURN, DRETURN, ISTORETOP, DSTORETOP, 
PUSHADRS, OLDEBP, OLDESP, IRESULT, INEGATIVE, DRESULT,
DNEGATIVE, IGLOAD, ILLOAD, DGLOAD, DLLOAD, ICONST,
DCONST, ITOD2, IADD, DADD, ISUB, DSUB, IMUL, DMUL, IDIV, 
DDIV, ISML, DSML, IBIG, DBIG, ISMLEQL, DSMLEQL, IEQLEQL, 
DEQLEQL, IBIGEQL, DBIGEQL, INOTEQL, DNOTEQL};

union IRType
{
    size_t opcode;
    double dValue;
    int iValue;
} instruction;
```
虚拟机利用计算栈stk存储中间变量，寄存器irx存储int类型函数返回值，drx存储double类型函数返回值，ipx存储当前指令位置，ebp存储函数调用栈帧的起始位置，esp存储当前函数调用栈栈顶位置。虚拟机内存memory和计算栈stk的存储单元类型均为union类型MemType
```
union MemType
{
    size_t address;
    double dValue;
    int iValue;
} data;
int irx;
double drx;
size_t ipx;
size_t ebp;
size_t esp;
std::vector<MemType> memory;
std::stack<MemType> stk;
```
Interpreter会在IR生成完毕后在代码序列末尾加上数条对main函数的调用指令，虚拟机执行时会从代码序列末端开始跳转至main函数执行。a.lq示例程序的IR指令如下
```
0 PUSHEBP
1 NEWEBP
2 ICONST
3 0
4 IGSTORE
5 4
6 ILLOAD
7 -2
8 ICONST
9 0
10 IEQLEQL
11 IFCMP
12 22
13 IGLOAD
14 4
15 ILLOAD
16 -2
17 IADD
18 IRETURN
19 JUMPBACK
20 JUMP
21 38
22 IGLOAD
23 4
24 ICONST
25 10
26 ISML
27 IFNCMP
28 38
29 IGLOAD
30 4
31 ICONST
32 1
33 IADD
34 IGSTORE
35 4
36 JUMP
37 22
38 JUMPBACK
39 PUSHEBP      // main
40 NEWEBP
41 ICONST
42 5
43 IGSTORE
44 3
45 MALLOC
46 ICONST
47 0
48 MALLOC
49 ISTORETOP
50 PUSHADRS
51 54
52 JUMP
53 0
54 OLDEBP
55 OLDESP
56 1
57 IRESULT
58 ILSTORE
59 1
60 ILLOAD
61 1
62 IRETURN
63 JUMPBACK
64 JUMPBACK
65 PUSHADRS     // Start point
66 69
67 JUMP
68 39
```
虚拟机指令的译指执行过程见Interpreter::exec()

### Ref
The Function Stack  
http://www.tenouk.com/Bufferoverflowc/Bufferoverflow2a.html
虚拟机随谈
http://rednaxelafx.iteye.com/blog/492667





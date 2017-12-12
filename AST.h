#ifndef _AST_H_
#define _AST_H_

#include <iostream>
#include <stack>
#include "symbolTable.h"


class ASTNode
{
public:
    enum { VARNODE=1, FUNCNODE, CALLNODE, ASSIGNNODE, IFNODE, WHILENODE, BREAKNODE, RETURNNODE, EXPRNODE, INTNODE, DOUBLENODE, VARVALNODE, OPERATORNODE };
    enum { TVOID=0, TINT, TDOUBLE, TINTARR, TDOUBLEARR, TFUNCVOID, TFUNCINT, TFUNCDOUBLE };
    int nodeType;
    ASTNode* next;
    ASTNode() {}
    ASTNode(int t) : nodeType(t), next(NULL) {}
    virtual ~ASTNode() {}
};

class VarNode : public ASTNode
{
public:
    size_t name;
    int varType;
    size_t arrLength;
    VarNode() : ASTNode::ASTNode() {}
    VarNode(size_t n, int vt, size_t l=0) : ASTNode::ASTNode(VARNODE), name(n), varType(vt), arrLength(l) {}
    virtual ~VarNode() {}
};

class FuncNode : public ASTNode
{
public:
    size_t name;
    int retType;
    ASTNode* params;
    ASTNode* lclStmts;
    int paramNum;
    FuncNode() : ASTNode::ASTNode() {}
    FuncNode(size_t n, int rt, int p=0) : ASTNode::ASTNode(FUNCNODE), name(n), retType(rt), params(NULL), lclStmts(NULL), paramNum(p) {}
    virtual ~FuncNode() {}
};

class CallNode : public ASTNode
{
public:
    size_t name;
    int retType;
    ASTNode* args;
    bool negativeFlag;
    CallNode() : ASTNode::ASTNode() {}
    CallNode(size_t n, int rt, bool f=false) : ASTNode::ASTNode(CALLNODE), name(n), retType(rt), args(NULL), negativeFlag(f) {}
    virtual ~CallNode() {}
};

class AssignNode : public ASTNode
{
public:
    size_t name;
    int varType;
    int arrIndex;
    ASTNode* value;
    AssignNode() : ASTNode::ASTNode() {}
    AssignNode(size_t n, int vt, int i=-1) : ASTNode::ASTNode(ASSIGNNODE), name(n), varType(vt), arrIndex(i), value(NULL) {}
    virtual ~AssignNode() {}
};

class IfNode : public ASTNode
{
public:
    ASTNode* value;
    ASTNode* trueStmts;
    ASTNode* falseStmts;
    IfNode() : ASTNode::ASTNode(IFNODE), value(NULL), trueStmts(NULL), falseStmts(NULL) {}
    virtual ~IfNode() {}
};

class WhileNode : public ASTNode
{
public:
    ASTNode* value;
    ASTNode* lclStmts;
    WhileNode() : ASTNode::ASTNode(WHILENODE), value(NULL), lclStmts(NULL) {}
    virtual ~WhileNode() {}
};

class BreakNode : public ASTNode
{
public:
    BreakNode() : ASTNode::ASTNode(BREAKNODE) {}
    virtual ~BreakNode() {}
};

class ReturnNode : public ASTNode
{
public:
    ASTNode* value;
    ReturnNode() : ASTNode::ASTNode(RETURNNODE), value(NULL) {}
    virtual ~ReturnNode() {}
};

class ExprNode : public ASTNode
{
public:
    ASTNode* value;
    int varType;
    ExprNode() : ASTNode::ASTNode(EXPRNODE), value(NULL), varType(0) {}
    virtual ~ExprNode() {}
};

class IntNode : public ASTNode
{
public:
    int value;
    bool negativeFlag;
    IntNode() : ASTNode::ASTNode() {}
    IntNode(int v, bool f=false) : ASTNode::ASTNode(INTNODE), value(v), negativeFlag(f) {}
    virtual ~IntNode() {}
};

class DoubleNode : public ASTNode
{
public:
    double value;
    bool negativeFlag;
    DoubleNode() : ASTNode::ASTNode() {}
    DoubleNode(double v, bool f=false) : ASTNode::ASTNode(DOUBLENODE), value(v), negativeFlag(f) {}
    virtual ~DoubleNode() {}
};

class VarValNode : public ASTNode
{
public:
    size_t name;
    int varType;
    int arrIndex;
    bool negativeFlag;
    VarValNode() : ASTNode::ASTNode() {}
    VarValNode(size_t n, int vt, int i=-1, bool f=false) : ASTNode::ASTNode(VARVALNODE), name(n), varType(vt), arrIndex(i), negativeFlag(f) {}
    virtual ~VarValNode() {}    
};

class OperatorNode : public ASTNode
{
public:
    int op;
    OperatorNode() : ASTNode::ASTNode() {}
    OperatorNode(int o) : ASTNode::ASTNode(OPERATORNODE), op(o) {}
    virtual ~OperatorNode() {}    
};



class AST
{
private:
    enum { VARNODE=1, FUNCNODE, CALLNODE, ASSIGNNODE, IFNODE, WHILENODE, BREAKNODE, RETURNNODE, EXPRNODE, INTNODE, DOUBLENODE, VARVALNODE, OPERATORNODE };
    enum { TVOID=0, TINT, TDOUBLE, TINTARR, TDOUBLEARR, TFUNCVOID, TFUNCINT, TFUNCDOUBLE };
    SymbolTable table;
    ASTNode* root;
    ASTNode** crtNode;
    std::stack<ASTNode*> stk;
    size_t IDCount;
    int breakFlag;
    bool isHigher(OperatorNode* a, OperatorNode* b);
    int getLevel(OperatorNode* node);
public:
    AST() : root(NULL), crtNode(NULL), IDCount(0), breakFlag(0) {}
    ASTNode* getRoot() { return root; }
    bool checkMain();
    bool addRoot();
    bool addFuncDef(const char* name, size_t nameLength, int typeFlag);
    bool endFuncDef();
    bool endParameters();
    bool addVariable(const char* name, size_t nameLength, int typeFlag, size_t arrLength=0);
    bool addFuncCall(const char* name, size_t nameLength, bool negativeFlag=false);
    bool endArguments();
    bool addAssign(const char* name, size_t nameLength, int arrIndex=-1);
    bool addExpr();
    bool endExpr();
    bool endAssign();
    bool addIf();
    bool endIfVal();
    bool endIfTrue();
    bool endIFFalse();
    bool addWhile();
    bool endWhileVal();
    bool endWhile();
    bool addBreak();
    bool addReturn();
    bool endReturn();
    bool addOperator(int op);
    bool addVarVal(const char* name, size_t nameLength, int arrIndex=-1, bool negativeFlag=false);
    bool addInt(int value, bool negativeFlag=false);
    bool addDouble(double value, bool negativeFlag=false);
    ~AST() {}
};





#endif
#ifndef _AST_H_
#define _AST_H_


#include <iostream>


class ASTNode
{
public:
    enum { VARNODE=1, FUNCNODE, CALLNODE, ASSIGNNODE, IFNODE, WHILENODE, BREAKNODE, RETURNNODE, EXPRNODE, INTNODE, DOUBLENODE, VARVALNODE, OPERATORNODE };
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
    FuncNode() : ASTNode::ASTNode() {}
    FuncNode(size_t n, int rt) : ASTNode::ASTNode(FUNCNODE), name(n), retType(rt), params(NULL), lclStmts(NULL) {}
    virtual ~FuncNode() {}
};

class CallNode : public ASTNode
{
public:
    size_t name;
    ASTNode* args;
    CallNode() : ASTNode::ASTNode() {}
    CallNode(size_t n) : ASTNode::ASTNode(CALLNODE), name(n), args(NULL) {}
    virtual ~CallNode() {}
};

class AssignNode : public ASTNode
{
public:
    size_t name;
    size_t arrIndex;
    ASTNode* value;
    AssignNode() : ASTNode::ASTNode() {}
    AssignNode(size_t n, size_t i) : ASTNode::ASTNode(ASSIGNNODE), name(n), arrIndex(i), value(NULL) {}
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
    ExprNode() : ASTNode::ASTNode(EXPRNODE), value(NULL) {}
    virtual ~ExprNode() {}
};

class IntNode : public ASTNode
{
public:
    int value;
    IntNode() : ASTNode::ASTNode() {}
    IntNode(int v) : ASTNode::ASTNode(INTNODE), value(v) {}
    virtual ~IntNode() {}
};

class DoubleNode : public ASTNode
{
public:
    double value;
    DoubleNode() : ASTNode::ASTNode() {}
    DoubleNode(double v) : ASTNode::ASTNode(DOUBLENODE), value(v) {}
    virtual ~DoubleNode() {}
};

class VarValNode : public ASTNode
{
public:
    size_t name;
    size_t arrIndex;
    VarValNode() : ASTNode::ASTNode() {}
    VarValNode(size_t n, size_t i) : ASTNode::ASTNode(VARVALNODE), name(n), arrIndex(i) {}
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
    int i;
public:
    void addRoot();
    void addVariable(const char* name, size_t nameLength, int typeFlag, size_t arrLength=0);
    void addFuncDef(const char* name, size_t nameLength, int typeFlag, size_t arrLength=0);
    void endFuncDef();
    void addParameter(const char* name, size_t nameLength, int typeFlag, size_t arrLength=0);
    void addFuncCall(const char* name, size_t nameLength);
    void addIf();
    void addElse();
    void endElse();
    void endIf();
    void addWhile();
    void endWhile();
    void addBreak();
    void addReturn();
    void addAssign(const char* name, size_t nameLength, size_t index=0);

    void addExpr();
    void addExpr1();
    void addExpr2();
    void addExpr3();
};





#endif
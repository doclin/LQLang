#include "AST.h"


//enum { TVOID=0, TINT, TDOUBLE, TINTARR, TDOUBLEARR, TFUNC };
//SymbolTable table;
//ASTNode* root;
//ASTNode** crtNode;
//std::stack<ASTNode*> stk;
//size_t IDCount;
//bool breakFlag;

//bool addRoot();
//bool addFuncDef(const char* name, size_t nameLength, int typeFlag);
//bool endFuncDef();
//bool endParameters();
//bool addVariable(const char* name, size_t nameLength, int typeFlag, size_t arrLength=0);
//bool addFuncCall(const char* name, size_t nameLength, bool negativeFlag=false);
//bool endArguments();
//bool addAssign(const char* name, size_t nameLength, int arrIndex=-1);
//bool addExpr();
//bool endExpr();
//bool endAssign();
//bool addIf();
//bool endIfVal();
//bool endIfTrue();
//bool endIFFalse();
//bool addWhile();
//bool endWhileVal();
//bool endWhile();
//bool addBreak();
//bool addReturn();
//bool endReturn();
//bool addOperator(int op);
//bool addVarVal(const char* name, size_t nameLength, int arrIndex=-1, bool negativeFlag=false);
//bool addInt(int value, bool negativeFlag=false);
//bool addDouble(double value, bool negativeFlag=false);

bool AST::addRoot()
{
    table.addScope();
    crtNode = &root;
    return true;
}

bool AST::addFuncDef(const char* name, size_t nameLength, int typeFlag)
{
    if(!table.addName(name, nameLength, IDCount, typeFlag+4))
        return false;
    *crtNode = new FuncNode(IDCount, typeFlag);
    stk.push(*crtNode);
    crtNode = &(static_cast<FuncNode*>(*crtNode) -> params);
    table.addScope();
    IDCount++;
    return true;
}

bool AST::endFuncDef()
{
    if(stk.top()->nodeType != FUNCNODE)
        return false;
    crtNode = &(stk.top()->next);
    stk.pop();
    table.popScope();
    return true;
}












bool AST::addVariable(const char* name, size_t nameLength, int typeFlag, size_t arrLength=0)
{
    if(table.addName())
    {
        *crtNode = new VarNode(IDCount++, typeFlag, arrLength);
        crtNode = &(*crtNode)->next;
        return true;
    }
    else
        return false;
}

bool AST::addParameter(const char* name, size_t nameLength, int typeFlag, size_t arrLength=0)
{
    if(table.addName())
    {
        *crtNode = new VarNode(IDCount++, typeFlag, arrLength);
        crtNode = &(*crtNode)->next;
        return true;
    }
    else
        return false;
}

bool AST::endParameter()
{
    //pop;
}

bool AST::addAssign(const char* name, size_t nameLength, size_t index=0)
{
    if(table.findName())
    {
        *crtNode = new AssignNode(IDCount++, index);
        crtNode = &static_cast<AssignNode*>(*crtNode)->value;
        return true;
    }
    else
        return false;    
}

bool AST::addExpr()
{
    *crtNode = new ExprNode();
    crtNode = &static_cast<ExprNode*>(*crtNode)->value;
    return true;
}

bool AST::endExpr()
{
    //pop
}

bool AST::endAssign()
{
    //pop
}

bool AST::addIf()
{
    *crtNode = new IfNode();
    crtNode = &static_cast<IfNode*>(*crtNode)->value;
    return true;
}



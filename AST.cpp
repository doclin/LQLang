#include "AST.h"

//void addRoot();
//void addVariable(const char* name, size_t nameLength, int typeFlag, size_t arrLength=0);
//void addFuncDef(const char* name, size_t nameLength, int typeFlag, size_t arrLength=0);
//void endFuncDef();
//void addParameter(const char* name, size_t nameLength, int typeFlag, size_t arrLength=0);
//void addFuncCall(const char* name, size_t nameLength);
//void addIf();
//void addElse();
//void endElse();
//void endIf();
//void addWhile();
//void endWhile();
//void addBreak();
//void addReturn();
//void addAssign(const char* name, size_t nameLength, size_t index=0);
//
//void addExpr();
//void addExpr1();
//void addExpr2();
//void addExpr3();

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

bool AST::addFuncDef(const char* name, size_t nameLength, int typeFlag)
{
    if(table.addName())
    {
        table.addScope();
        *crtNode = new FuncNode(IDCount++, typeFlag);
        crtNode = &static_cast<FuncNode*>(*crtNode)->params;
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



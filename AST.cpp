#include "AST.h"


bool AST::addRoot()
{
    table.addScope();
    crtNode = &root;
    return true;
}

bool AST::addFuncDef(const char* name, size_t nameLength, int typeFlag)
{
    if(!table.addName(name, nameLength, IDCount, typeFlag+5))
        return false;
    *crtNode = new FuncNode(IDCount, typeFlag);
    stk.push(*crtNode);
    crtNode = &static_cast<FuncNode*>(*crtNode) -> params;
    table.addScope();
    IDCount++;
    return true;
}

bool AST::endFuncDef()
{
    if(stk.top()->nodeType != FUNCNODE)
        return false;
    int i = 0;
    for(ASTNode* n=static_cast<FuncNode*>(stk.top())->params; n!=NULL; n=n->next)
        i++;
    static_cast<FuncNode*>(stk.top())->paramNum = i;
    crtNode = &(stk.top()->next);
    stk.pop();
    table.popScope();
    return true;
}

bool AST::endParameters()
{
    if(stk.top()->nodeType != FUNCNODE)
        return false;
    crtNode = &static_cast<FuncNode*>(stk.top())->lclStmts;
    return true;
}

bool AST::addVariable(const char* name, size_t nameLength, int typeFlag, size_t arrLength)
{
    if(!table.addName(name, nameLength, IDCount, typeFlag))
        return false;
    *crtNode = new VarNode(IDCount, typeFlag, arrLength);
    crtNode = &(*crtNode)->next;
    IDCount++;
    return true;
}

bool AST::addFuncCall(const char* name, size_t nameLength, bool negativeFlag)
{
    size_t tmpID;
    int tmpTF;
    if(!table.findName(name, nameLength, tmpID, tmpTF))
        return false;
    if(tmpTF!=TFUNCINT && tmpTF!=TFUNCDOUBLE && tmpTF!=TFUNCVOID)
        return false;
    *crtNode = new CallNode(tmpID, tmpTF-5, negativeFlag);
    stk.push(*crtNode);
    crtNode = &static_cast<CallNode*>(*crtNode)->args;
    return true;
}

bool AST::endArguments()
{
    if(stk.top()->nodeType != CALLNODE)
        return false;
    crtNode = &stk.top()->next;
    stk.pop();
    return true;
}

bool AST::addAssign(const char* name, size_t nameLength, int arrIndex)
{
    size_t tmpID;
    int tmpTF;
    if(!table.findName(name, nameLength, tmpID, tmpTF))
        return false;
    if(tmpTF==TFUNCDOUBLE || tmpTF==TFUNCINT || tmpTF==TFUNCVOID)
        return false;
    if(arrIndex==-1 && (tmpTF==TINTARR || tmpTF==TDOUBLEARR))
        return false;
    if(arrIndex>=0 && (tmpTF==TINT || tmpTF==TDOUBLE))
        return false;
    if(tmpTF==TINTARR || tmpTF==TDOUBLEARR)
        tmpTF = tmpTF - 2;
    *crtNode = new AssignNode(tmpID, tmpTF, arrIndex);
    stk.push(*crtNode);
    crtNode = &static_cast<AssignNode*>(*crtNode)->value;
    return true;
}

bool AST::addExpr()
{
    *crtNode = new ExprNode();
    stk.push(*crtNode);
    crtNode = &static_cast<ExprNode*>(*crtNode)->value;
    return true;
}

bool AST::endExpr()
{
    if(stk.top()->nodeType != EXPRNODE)
        return false;
    ASTNode* operandList = &ASTNode();
    ASTNode* crtOperand = operandList;
    ASTNode* operatorList = &ASTNode();
    ASTNode* crtOperator = operatorList;
    for(ASTNode* origin = static_cast<ExprNode*>(stk.top())->value; origin != NULL; origin = origin->next)
    {
        if(origin->nodeType==CALLNODE && static_cast<CallNode*>(origin)->retType==TVOID)
            return false;
        if(origin->nodeType==INTNODE || origin->nodeType==DOUBLENODE || origin->nodeType==CALLNODE || origin->nodeType==VARVALNODE)
        {
            crtOperand -> next = origin;
            crtOperand = crtOperand -> next;
        }
        else

    }



    crtNode = &stk.top()->next;
    stk.pop();
    return true;
}

bool AST::endAssign()
{
    if(stk.top()->nodeType != ASSIGNNODE)
        return false;
    crtNode = &stk.top()->next;
    stk.pop();
    return true;
}

bool AST::addIf()
{
    *crtNode = new IfNode();
    stk.push(*crtNode);
    crtNode = &static_cast<IfNode*>(*crtNode)->value;
    return true;
}

bool AST::endIfVal()
{
    if(stk.top()->nodeType != IFNODE)
        return false;
    crtNode = &static_cast<IfNode*>(stk.top())->trueStmts;
    table.addScope();
    return true;
}

bool AST::endIfTrue()
{
    if(stk.top()->nodeType != IFNODE)
        return false;
    crtNode = &static_cast<IfNode*>(stk.top())->falseStmts;
    table.popScope();
    table.addScope();
    return true;
}

bool AST::endIFFalse()
{
    if(stk.top()->nodeType != IFNODE)
        return false;
    crtNode = &stk.top() -> next;
    stk.pop();
    table.popScope();
    return true;
}

bool AST::addWhile()
{
    *crtNode = new WhileNode();
    stk.push(*crtNode);
    crtNode = &static_cast<WhileNode*>(*crtNode)->value;
    return true;
}

bool AST::endWhileVal()
{
    if(stk.top()->nodeType != WHILENODE)
        return false;
    crtNode = &static_cast<WhileNode*>(stk.top())->lclStmts;
    table.addScope();
    breakFlag = true;
    return true;
}

bool AST::endWhile()
{
    if(stk.top()->nodeType != WHILENODE)
        return false;
    crtNode = &stk.top() -> next;
    stk.pop();
    table.popScope();
    breakFlag = false;
    return true;
}

bool AST::addBreak()
{
    if(!breakFlag)
        return false;
    *crtNode = new BreakNode();
    crtNode = &(*crtNode) -> next;
    return true;
}

bool AST::addReturn()
{
    *crtNode = new ReturnNode();
    stk.push(*crtNode);
    crtNode = &static_cast<ReturnNode*>(*crtNode)->value;
    return true;
}

bool AST::endReturn()
{
    if(stk.top()->nodeType != RETURNNODE)
        return false;
    crtNode = &stk.top() -> next;
    stk.pop();
    return true;
}

bool AST::addOperator(int op)
{
    *crtNode = new OperatorNode(op);
    crtNode = &(*crtNode) -> next;
    return true;
}

bool AST::addVarVal(const char* name, size_t nameLength, int arrIndex, bool negativeFlag)
{
    size_t tmpID;
    int tmpTF;
    if(!table.findName(name, nameLength, tmpID, tmpTF))
        return false;
    if(tmpTF==TFUNCDOUBLE || tmpTF==TFUNCINT || tmpTF==TFUNCVOID)
        return false;
    if(arrIndex==-1 && (tmpTF==TINTARR || tmpTF==TDOUBLEARR))
        return false;
    if(arrIndex>=0 && (tmpTF==TINT || tmpTF==TDOUBLE))
        return false;
    if(tmpTF==TINTARR || tmpTF==TDOUBLEARR)
        tmpTF = tmpTF - 2;
    *crtNode = new VarValNode(tmpID, tmpTF, arrIndex, negativeFlag);
    crtNode = &(*crtNode) -> next;
    return true;
}

bool AST::addInt(int value, bool negativeFlag)
{
    *crtNode = new IntNode(value, negativeFlag);
    crtNode = &(*crtNode) -> next;
    return true;
}

bool AST::addDouble(double value, bool negativeFlag)
{
    *crtNode = new DoubleNode(value, negativeFlag);
    crtNode = &(*crtNode) -> next;
    return true;
}

#include "AST.h"
#include <cstring>


bool AST::addRoot()
{
    table.addScope();
    crtNode = &root;
    return true;
}


bool AST::checkMain()
{
    size_t tmpID;
    int tmpTF;
    if(!table.findName("main", 4, tmpID, tmpTF))
        return false;    
}

bool AST::addFuncDef(const char* name, size_t nameLength, int typeFlag)
{
    size_t funcName = IDCount;
    if(nameLength==4 && memcmp(name, "main", 4)==0)
        funcName = 0xabcdef12;
    if(!table.addName(name, nameLength, funcName, typeFlag+5))
        return false;
    *crtNode = new FuncNode(funcName, typeFlag);
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
    *crtNode = new ReturnNode();
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

    ASTNode* operandList = new ASTNode();
    ASTNode* crtOperand = operandList;
    std::stack<OperatorNode*> operatorStack;
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
        {
            if(operatorStack.empty() || static_cast<OperatorNode*>(origin)->op=='(' || isHigher(static_cast<OperatorNode*>(origin), operatorStack.top()))
            {
                operatorStack.push(static_cast<OperatorNode*>(origin));
            }
            else
            {
                if(static_cast<OperatorNode*>(origin)->op==')')
                {
                    while(operatorStack.top()->op != '(')
                    {
                        crtOperand -> next = operatorStack.top();
                        crtOperand = crtOperand -> next;
                        operatorStack.pop();
                    }
                    operatorStack.pop();
                }
                else
                {
                    while((!operatorStack.empty()) && getLevel(static_cast<OperatorNode*>(origin))<=getLevel(operatorStack.top()) && operatorStack.top()->op!='(')
                    {
                        crtOperand -> next = operatorStack.top();
                        crtOperand = crtOperand -> next;
                        operatorStack.pop();
                    }
                    operatorStack.push(static_cast<OperatorNode*>(origin));
                }
            }
        }
    }
    while(!operatorStack.empty())
    {
        crtOperand -> next = operatorStack.top();
        crtOperand = crtOperand -> next;
        operatorStack.pop();       
    }
    crtOperand -> next = NULL;
    static_cast<ExprNode*>(stk.top())->value = operandList -> next;

    crtNode = &stk.top()->next;
    stk.pop();
    return true;
}

bool AST::isHigher(OperatorNode* a, OperatorNode* b)
{
    return (getLevel(a) > getLevel(b));
}

int AST::getLevel(OperatorNode* node)
{
    int op = node -> op;
    if(op=='<' || op=='>' || op==1000+'<' || op==1000+'>' || op==1000+'=' || op==1000+'!')
        return 1;
    else if(op=='+' || op=='-')
        return 2;
    else if(op=='*' || op=='/')
        return 3;
    else if(op=='(')
        return 4;
    else if(op==')')
        return 0;
    else
        return -1;
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
    breakFlag++;
    return true;
}

bool AST::endWhile()
{
    if(stk.top()->nodeType != WHILENODE)
        return false;
    crtNode = &stk.top() -> next;
    stk.pop();
    table.popScope();
    breakFlag--;
    return true;
}

bool AST::addBreak()
{
    if(breakFlag <= 0)
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

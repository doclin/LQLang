#include "interpreter.h"


void Interpreter::interpret(AST& tree)
{
    //if(!tree.checkMain())
        //return;
    root = tree.getRoot();
    translateIR();

    size_t jumpAddress = addressTable.find(0xabcdef12)->second.first;
    instruction.opcode = PUSHADRS;
    IR.push_back(instruction);
    instruction.opcode = IR.size() + 3;
    IR.push_back(instruction);
    instruction.opcode = JUMP;
    IR.push_back(instruction);
    instruction.opcode = jumpAddress;
    IR.push_back(instruction);

    exec();
    std::cout << "Result:" << irx << std::endl;
}

void Interpreter::showInterpretation(AST& tree)
{
    //if(!tree.checkMain())
    //    return;
    root = tree.getRoot();
    translateIR();

    size_t jumpAddress = addressTable.find(0xabcdef12)->second.first;
    instruction.opcode = PUSHADRS;
    IR.push_back(instruction);
    instruction.opcode = IR.size() + 3;
    IR.push_back(instruction);
    instruction.opcode = JUMP;
    IR.push_back(instruction);
    instruction.opcode = jumpAddress;
    IR.push_back(instruction);

    showIR();
}




void Interpreter::translateIR()
{
    for(ASTNode* n=root; n!=NULL; n=n->next)
    {
        if(n->nodeType == VARNODE)
        {
            int varType = static_cast<VarNode*>(n)->varType;
            size_t name = static_cast<VarNode*>(n)->name;
            if(varType==TINT || varType==TDOUBLE)
            {
                addressTable.insert(std::pair<size_t, std::pair<int, bool> >(name, std::pair<int, bool>(memory.size(), true)));
                memory.push_back(data);
            }
            else if(varType==TINTARR || varType==TDOUBLEARR)
            {
                addressTable.insert(std::pair<size_t, std::pair<int, bool> >(name, std::pair<int, bool>(memory.size(), true)));
                memory.resize(memory.size() + static_cast<VarNode*>(n)->arrLength);
            }
            else
                return;
        }
        else if(n->nodeType == FUNCNODE)
        {
            size_t name = static_cast<FuncNode*>(n)->name;
            addressTable.insert(std::pair<size_t, std::pair<int, bool> >(name, std::pair<int, bool>(IR.size(), true)));
            instruction.opcode = PUSHEBP;
            IR.push_back(instruction);
            instruction.opcode = NEWEBP;
            IR.push_back(instruction);
            int paramNum = static_cast<FuncNode*>(n)->paramNum;
            int i = -1;
            int local = 1;
            for(ASTNode* no=static_cast<FuncNode*>(n)->params; no!=NULL; no=no->next)
            {
                size_t name = static_cast<VarNode*>(no)->name;
                addressTable.insert(std::pair<size_t, std::pair<int, bool> >(name, std::pair<int, bool>(i-paramNum, false)));
                i++;
            }
            translateLclStmts(static_cast<FuncNode*>(n)->lclStmts, local);    
        }
        else
            return;
    }
}

void Interpreter::translateLclStmts(ASTNode* n, int& local)
{
    for( ; n!=NULL; n=n->next)
    {
        if(n->nodeType == VARNODE)
        {
            int varType = static_cast<VarNode*>(n)->varType;
            size_t name = static_cast<VarNode*>(n)->name;
            if(varType==TINT || varType==TDOUBLE)
            {
                addressTable.insert(std::pair<size_t, std::pair<int, bool> >(name, std::pair<int, bool>(local, false)));
                instruction.opcode = MALLOC;
                IR.push_back(instruction);
                local++;
            }
            else if(varType==TINTARR || varType==TDOUBLEARR)
            {
                addressTable.insert(std::pair<size_t, std::pair<int, bool> >(name, std::pair<int, bool>(local, false)));
                instruction.opcode = MALLOCARR;
                IR.push_back(instruction);
                instruction.opcode = static_cast<VarNode*>(n)->arrLength;
                IR.push_back(instruction);
                local += static_cast<VarNode*>(n)->arrLength;
            }
            else
                return;                                        
        }
        else if(n->nodeType == CALLNODE)
            translateCall(n);
        else if(n->nodeType == ASSIGNNODE)
        {
            int varType = static_cast<AssignNode*>(n)->varType;
            int arrIndex = static_cast<AssignNode*>(n)->arrIndex;
            std::pair<int, bool> result = addressTable.find(static_cast<AssignNode*>(n)->name)->second;
            int address = result.first;
            bool global = result.second;            
            translateExpr(static_cast<AssignNode*>(n)->value);
            int exprType = static_cast<ExprNode*>(static_cast<AssignNode*>(n)->value)->varType;
            if(varType==TINT && exprType==TDOUBLE)
            {
                instruction.opcode = DTOI;
                IR.push_back(instruction);
            }
            else if(varType==TDOUBLE && exprType==TINT)
            {
                instruction.opcode = ITOD;
                IR.push_back(instruction);
            }
            if(varType==TINT)
            {
                if(arrIndex == -1)
                {
                    if(global)
                    {
                        instruction.opcode = IGSTORE;
                        IR.push_back(instruction);
                        instruction.opcode = address;
                        IR.push_back(instruction);
                    }
                    else
                    {
                        instruction.opcode = ILSTORE;
                        IR.push_back(instruction);
                        instruction.iValue = address;
                        IR.push_back(instruction);
                    }
                }
                else
                {
                    if(global)
                    {
                        instruction.opcode = IGSTORE;
                        IR.push_back(instruction);
                        instruction.opcode = address + arrIndex;
                        IR.push_back(instruction);
                    }
                    else
                    {
                        instruction.opcode = ILSTORE;
                        IR.push_back(instruction);
                        instruction.iValue = address + arrIndex;
                        IR.push_back(instruction);
                    }
                }
            }
            else
            {
                if(arrIndex == -1)
                {
                    if(global)
                    {
                        instruction.opcode = DGSTORE;
                        IR.push_back(instruction);
                        instruction.opcode = address;
                        IR.push_back(instruction);
                    }
                    else
                    {
                        instruction.opcode = DLSTORE;
                        IR.push_back(instruction);
                        instruction.iValue = address;
                        IR.push_back(instruction);
                    }
                }
                else
                {
                    if(global)
                    {
                        instruction.opcode = DGSTORE;
                        IR.push_back(instruction);
                        instruction.opcode = address + arrIndex;
                        IR.push_back(instruction);
                    }
                    else
                    {
                        instruction.opcode = DLSTORE;
                        IR.push_back(instruction);
                        instruction.iValue = address + arrIndex;
                        IR.push_back(instruction);
                    }
                }
            }
        }
        else if(n->nodeType == IFNODE)
        {
            size_t tmpAddress;
            size_t tmpAddress1;
            translateExpr(static_cast<IfNode*>(n)->value);
            instruction.opcode = IFCMP;
            IR.push_back(instruction);
            tmpAddress = IR.size();
            IR.push_back(instruction);
            translateLclStmts(static_cast<IfNode*>(n)->falseStmts, local);
            instruction.opcode = JUMP;
            IR.push_back(instruction);
            tmpAddress1 = IR.size();
            IR.push_back(instruction);
            IR[tmpAddress].opcode = IR.size();
            translateLclStmts(static_cast<IfNode*>(n)->trueStmts, local);
            IR[tmpAddress1].opcode = IR.size();
        }
        else if(n->nodeType == WHILENODE)
        {
            size_t tmpAddress;
            size_t tmpAddress1;
            tmpAddress = IR.size();
            translateExpr(static_cast<WhileNode*>(n)->value);
            instruction.opcode = IFNCMP;
            IR.push_back(instruction);
            tmpAddress1 = IR.size();
            IR.push_back(instruction);
            translateLclStmts(static_cast<WhileNode*>(n)->lclStmts, local);
            instruction.opcode = JUMP;
            IR.push_back(instruction);
            instruction.opcode = tmpAddress;
            IR.push_back(instruction);
            IR[tmpAddress1].opcode = IR.size();
            while(!breakpoints.empty())
            {
                IR[breakpoints.back()].opcode = IR.size();
                breakpoints.pop_back();
            }
        }
        else if(n->nodeType == BREAKNODE)
        {
            instruction.opcode = JUMP;
            IR.push_back(instruction);
            breakpoints.push_back(IR.size());
            IR.push_back(instruction);
        }
        else if(n->nodeType == RETURNNODE)
        {
            if(static_cast<ReturnNode*>(n)->value == NULL)
            {
                instruction.opcode = JUMPBACK;
                IR.push_back(instruction);
            }
            else
            {
                translateExpr(static_cast<ReturnNode*>(n)->value);
                int varType = static_cast<ExprNode*>(static_cast<ReturnNode*>(n)->value)->varType;
                if(varType == TINT)
                    instruction.opcode = IRETURN;
                else
                    instruction.opcode = DRETURN;
                IR.push_back(instruction);
                instruction.opcode = JUMPBACK;
                IR.push_back(instruction);
            }
        }
    }
}

void Interpreter::translateCall(ASTNode* n)
{
    int space = 0;
    for(ASTNode* node=static_cast<CallNode*>(n)->args; node!=NULL; node=node->next)
    {
        translateExpr(node);
        int typeFlag = static_cast<ExprNode*>(node)->varType;
        instruction.opcode = MALLOC;
        IR.push_back(instruction);
        if(typeFlag == TINT)
            instruction.opcode = ISTORETOP;
        else
            instruction.opcode = DSTORETOP;
        IR.push_back(instruction);
        space++;
    }
    instruction.opcode = PUSHADRS;
    IR.push_back(instruction);
    instruction.opcode = IR.size() + 3;
    IR.push_back(instruction);
    size_t jumpAddress = addressTable.find(static_cast<CallNode*>(n)->name)->second.first;
    instruction.opcode = JUMP;
    IR.push_back(instruction);
    instruction.opcode = jumpAddress;
    IR.push_back(instruction);
    instruction.opcode = OLDEBP;
    IR.push_back(instruction);
    instruction.opcode = OLDESP;
    IR.push_back(instruction);
    instruction.opcode = space;
    IR.push_back(instruction);
}


void Interpreter::translateExpr(ASTNode* n)
{
    for(ASTNode* node=static_cast<ExprNode*>(n)->value; node!=NULL; node=node->next)
    {
        if(node->nodeType == CALLNODE)
        {
            translateCall(node);
            int typeFlag = static_cast<CallNode*>(node)->retType;
            bool negativeFlag = static_cast<CallNode*>(node)->negativeFlag;
            if(typeFlag == TINT)
            {
                instruction.opcode = IRESULT;
                IR.push_back(instruction);
                if(negativeFlag)
                {
                    instruction.opcode = INEGATIVE;
                    IR.push_back(instruction);
                }
            }
            else
            {
                instruction.opcode = DRESULT;
                IR.push_back(instruction);
                if(negativeFlag)
                {
                    instruction.opcode = DNEGATIVE;
                    IR.push_back(instruction);
                }
            }
            typeStack.push(typeFlag);
        }
        else if(node->nodeType == VARVALNODE)
        {
            int typeFlag = static_cast<VarValNode*>(node)->varType;
            bool negativeFlag = static_cast<VarValNode*>(node)->negativeFlag;
            int arrIndex = static_cast<VarValNode*>(node)->arrIndex;
            std::pair<int, bool> result = addressTable.find(static_cast<VarValNode*>(node)->name)->second;
            int address = result.first;
            bool global = result.second;
            if(typeFlag==TINT)
            {
                if(arrIndex == -1)
                {
                    if(global)
                    {
                        instruction.opcode = IGLOAD;
                        IR.push_back(instruction);
                        instruction.opcode = address;
                        IR.push_back(instruction);
                    }
                    else
                    {
                        instruction.opcode = ILLOAD;
                        IR.push_back(instruction);
                        instruction.iValue = address;
                        IR.push_back(instruction);
                    }
                }
                else
                {
                    if(global)
                    {
                        instruction.opcode = IGLOAD;
                        IR.push_back(instruction);
                        instruction.opcode = address + arrIndex;
                        IR.push_back(instruction);
                    }
                    else
                    {
                        instruction.opcode = ILLOAD;
                        IR.push_back(instruction);
                        instruction.iValue = address + arrIndex;
                        IR.push_back(instruction);
                    }
                }
                if(negativeFlag)
                {
                    instruction.opcode = INEGATIVE;
                    IR.push_back(instruction);
                }
            }
            else
            {
                if(arrIndex == -1)
                {
                    if(global)
                    {
                        instruction.opcode = DGLOAD;
                        IR.push_back(instruction);
                        instruction.opcode = address;
                        IR.push_back(instruction);
                    }
                    else
                    {
                        instruction.opcode = DLLOAD;
                        IR.push_back(instruction);
                        instruction.iValue = address;
                        IR.push_back(instruction);
                    }
                }
                else
                {
                    if(global)
                    {
                        instruction.opcode = DGLOAD;
                        IR.push_back(instruction);
                        instruction.opcode = address + arrIndex;
                        IR.push_back(instruction);
                    }
                    else
                    {
                        instruction.opcode = DLLOAD;
                        IR.push_back(instruction);
                        instruction.iValue = address + arrIndex;
                        IR.push_back(instruction);
                    }
                }
                if(negativeFlag)
                {
                    instruction.opcode = DNEGATIVE;
                    IR.push_back(instruction);
                }
            }
            typeStack.push(typeFlag);
        }
        else if(node->nodeType == INTNODE)
        {
            int value = static_cast<IntNode*>(node)->value;
            bool negativeFlag = static_cast<IntNode*>(node)->negativeFlag;
            instruction.opcode = ICONST;
            IR.push_back(instruction);
            instruction.iValue = value;
            IR.push_back(instruction);
            if(negativeFlag)
            {
                instruction.opcode = INEGATIVE;
                IR.push_back(instruction);
            }
            typeStack.push(TINT);
        }
        else if(node->nodeType == DOUBLENODE)
        {
            double value = static_cast<DoubleNode*>(node)->value;
            bool negativeFlag = static_cast<DoubleNode*>(node)->negativeFlag;
            instruction.opcode = DCONST;
            IR.push_back(instruction);
            instruction.dValue = value;
            IR.push_back(instruction);
            if(negativeFlag)
            {
                instruction.opcode = DNEGATIVE;
                IR.push_back(instruction);
            }
            typeStack.push(TDOUBLE);
        }
        else if(node->nodeType == OPERATORNODE)
        {
            int op = static_cast<OperatorNode*>(node)->op;
            int rightType = typeStack.top();
            typeStack.pop();
            int leftType = typeStack.top();
            typeStack.pop();
            int resultType;
            if(leftType==TINT && rightType==TDOUBLE)
            {
                instruction.opcode = ITOD2;
                IR.push_back(instruction);
                resultType = TDOUBLE;
            }
            else if(leftType==TDOUBLE && rightType==TINT)
            {
                instruction.opcode = ITOD;
                IR.push_back(instruction);
                resultType = TDOUBLE;
            }
            else if(leftType == TINT)
                resultType = TINT;
            else
                resultType = TDOUBLE;

                
            if(op=='+' && resultType==TINT)
            {
                instruction.opcode = IADD;
                IR.push_back(instruction);
            }
            else if(op=='+' && resultType==TDOUBLE)
            {
                instruction.opcode = DADD;
                IR.push_back(instruction);
            }
            else if(op=='-' && resultType==TINT)
            {
                instruction.opcode = ISUB;
                IR.push_back(instruction);
            }
            else if(op=='-' && resultType==TDOUBLE)
            {
                instruction.opcode = DSUB;
                IR.push_back(instruction);
            }            
            else if(op=='*' && resultType==TINT)
            {
                instruction.opcode = IMUL;
                IR.push_back(instruction);
            }
            else if(op=='*' && resultType==TDOUBLE)
            {
                instruction.opcode = DMUL;
                IR.push_back(instruction);
            }
            else if(op=='/' && resultType==TINT)
            {
                instruction.opcode = IDIV;
                IR.push_back(instruction);
            }
            else if(op=='/' && resultType==TDOUBLE)
            {
                instruction.opcode = DDIV;
                IR.push_back(instruction);
            }
            else if(op=='<' && resultType==TINT)
            {
                instruction.opcode = ISML;
                IR.push_back(instruction);
            }
            else if(op=='<' && resultType==TDOUBLE)
            {
                instruction.opcode = DSML;
                IR.push_back(instruction);
            }            
            else if(op=='>' && resultType==TINT)
            {
                instruction.opcode = IBIG;
                IR.push_back(instruction);
            }
            else if(op=='>' && resultType==TDOUBLE)
            {
                instruction.opcode = DBIG;
                IR.push_back(instruction);
            }
            else if(op=='<'+1000 && resultType==TINT)
            {
                instruction.opcode = ISMLEQL;
                IR.push_back(instruction);
            }
            else if(op=='<'+1000 && resultType==TDOUBLE)
            {
                instruction.opcode = DSMLEQL;
                IR.push_back(instruction);
            }
            else if(op=='='+1000 && resultType==TINT)
            {
                instruction.opcode = IEQLEQL;
                IR.push_back(instruction);
            }
            else if(op=='='+1000 && resultType==TDOUBLE)
            {
                instruction.opcode = DEQLEQL;
                IR.push_back(instruction);
            }            
            else if(op=='>'+1000 && resultType==TINT)
            {
                instruction.opcode = IBIGEQL;
                IR.push_back(instruction);
            }
            else if(op=='>'+1000 && resultType==TDOUBLE)
            {
                instruction.opcode = DBIGEQL;
                IR.push_back(instruction);
            }
            else if(op=='!'+1000 && resultType==TINT)
            {
                instruction.opcode = INOTEQL;
                IR.push_back(instruction);
            }
            else if(op=='!'+1000 && resultType==TDOUBLE)
            {
                instruction.opcode = DNOTEQL;
                IR.push_back(instruction);
            }

            if(op=='<' || op=='>' || op=='<'+1000 || op=='='+1000 || op=='>'+1000 || op=='!'+1000) 
                resultType = TINT;
            typeStack.push(resultType);
        }
    }
    static_cast<ExprNode*>(n)->varType = typeStack.top();
    typeStack.pop();
}

void Interpreter::showIR()
{
    using namespace std;

    ebp = memory.size();
    while(ipx < IR.size())
    {
        switch(IR[ipx].opcode)
        {
        case PUSHEBP:
        {
            cout << ipx++ << " PUSHEBP" << endl;
            break;
        }
        case NEWEBP:
        {
            cout << ipx++ << " NEWEBP" << endl;
            break;
        }
        case MALLOC:
        {
            cout << ipx++ << " MALLOC" << endl;
            break;
        }        
        case MALLOCARR:
        {
            cout << ipx++ << " MALLOCARR" << endl;
            cout << ipx++ << " "; cout << IR[ipx-1].opcode << endl;
            break;
        }
        case DTOI:
        {
            cout << ipx++ << " DTOI" << endl;
            break;
        }
        case ITOD:
        {
            cout << ipx++ << " ITOD" << endl;
            break;
        }
        case IGSTORE:
        {
            cout << ipx++ << " IGSTORE" << endl;
            cout << ipx++ << " "; cout << IR[ipx-1].opcode << endl;
            break;
        }        
        case ILSTORE:
        {
            cout << ipx++ << " ILSTORE" << endl;
            cout << ipx++ << " "; cout << IR[ipx-1].iValue << endl;
            break;
        }
        case DGSTORE:
        {
            cout << ipx++ << " DGSTORE" << endl;
            cout << ipx++ << " "; cout << IR[ipx-1].opcode << endl;
            break;
        }
        case DLSTORE:
        {
            cout << ipx++ << " DLSTORE" << endl;
            cout << ipx++ << " "; cout << IR[ipx-1].iValue << endl;            
            break;
        }
        case IFCMP:
        {
            cout << ipx++ << " IFCMP" << endl;
            cout << ipx++ << " "; cout << IR[ipx-1].opcode << endl;
            break;
        }        
        case IFNCMP:
        {
            cout << ipx++ << " IFNCMP" << endl;
            cout << ipx++ << " "; cout << IR[ipx-1].opcode << endl;
            break;
        }
        case JUMP:
        {
            cout << ipx++ << " JUMP" << endl;
            cout << ipx++ << " "; cout << IR[ipx-1].opcode << endl;
            break;
        }
        case JUMPBACK:
        {
            cout << ipx++ << " JUMPBACK" << endl;
            break;
        }
        case IRETURN:
        {
            cout << ipx++ << " IRETURN" << endl;
            break;
        }        
        case DRETURN:
        {
            cout << ipx++ << " DRETURN" << endl;
            break;
        }
        case ISTORETOP:
        {
            cout << ipx++ << " ISTORETOP" << endl;
            break;
        }
        case DSTORETOP:
        {
            cout << ipx++ << " DSTORETOP" << endl;
            break;
        }
        case PUSHADRS:
        {
            cout << ipx++ << " PUSHADRS" << endl;
            cout << ipx++ << " "; cout << IR[ipx-1].opcode << endl;
            break;
        }        
        case OLDEBP:
        {
            cout << ipx++ << " OLDEBP" << endl;
            break;
        }
        case OLDESP:
        {
            cout << ipx++ << " OLDESP" << endl;
            cout << ipx++ << " "; cout << IR[ipx-1].opcode << endl;
            break;
        }
        case IRESULT:
        {
            cout << ipx++ << " IRESULT" << endl;        
            break;
        }
        case INEGATIVE:
        {
            cout << ipx++ << " INEGATIVE" << endl;
            break;
        }        
        case DRESULT:
        {
            cout << ipx++ << " DRESULT" << endl;
            break;
        }
        case DNEGATIVE:
        {
            cout << ipx++ << " DNEGATIVE" << endl;
            break;
        }
        case IGLOAD:
        {
            cout << ipx++ << " IGLOAD" << endl;
            cout << ipx++ << " "; cout << IR[ipx-1].opcode << endl;
            break;
        }
        case ILLOAD:
        {
            cout << ipx++ << " ILLOAD" << endl;
            cout << ipx++ << " "; cout << IR[ipx-1].iValue << endl;
            break;
        }        
        case DGLOAD:
        {
            cout << ipx++ << " DGLOAD" << endl;
            cout << ipx++ << " "; cout << IR[ipx-1].opcode << endl;
            break;
        }
        case DLLOAD:
        {
            cout << ipx++ << " DLLOAD" << endl;
            cout << ipx++ << " "; cout << IR[ipx-1].iValue << endl;
            break;
        }
        case ICONST:
        {
            cout << ipx++ << " ICONST" << endl;
            cout << ipx++ << " "; cout << IR[ipx-1].iValue << endl;
            break;
        }
        case DCONST:
        {
            cout << ipx++ << " DCONST" << endl;
            cout << ipx++ << " "; cout << IR[ipx-1].dValue << endl;
            break;
        }        
        case ITOD2:
        {
            cout << ipx++ << " ITOD2" << endl;
            break;
        }
        case IADD:
        {
            cout << ipx++ << " IADD" << endl;
            break;
        }
        case DADD:
        {
            cout << ipx++ << " DADD" << endl;        
            break;
        }
        case ISUB:
        {
            cout << ipx++ << " ISUB" << endl;
            break;
        }        
        case DSUB:
        {
            cout << ipx++ << " DSUB" << endl;
            break;
        }
        case IMUL:
        {
            cout << ipx++ << " IMUL" << endl;
            break;
        }
        case DMUL:
        {
            cout << ipx++ << " DMUL" << endl;
            break;
        }
        case IDIV:
        {
            cout << ipx++ << " IDIV" << endl;
            break;
        }        
        case DDIV:
        {
            cout << ipx++ << " DDIV" << endl;
            break;
        }
        case ISML:
        {
            cout << ipx++ << " ISML" << endl;
            break;
        }
        case DSML:
        {
            cout << ipx++ << " DSML" << endl;
            break;
        }
        case IBIG:
        {
            cout << ipx++ << " IBIG" << endl;
            break;
        }        
        case DBIG:
        {
            cout << ipx++ << " DBIG" << endl;
            break;
        }
        case ISMLEQL:
        {
            cout << ipx++ << " ISMLEQL" << endl;        
            break;
        }
        case DSMLEQL:
        {
            cout << ipx++ << " DSMLEQL" << endl;
            break;
        }        
        case IEQLEQL:
        {
            cout << ipx++ << " IEQLEQL" << endl;
            break;
        }
        case DEQLEQL:
        {
            cout << ipx++ << " DEQLEQL" << endl;
            break;
        }
        case IBIGEQL:
        {
            cout << ipx++ << " IBIGEQL" << endl;
            break;
        }
        case DBIGEQL:
        {
            cout << ipx++ << " DBIGEQL" << endl;
            break;
        }        
        case INOTEQL:
        {
            cout << ipx++ << " INOTEQL" << endl;
            break;
        }
        case DNOTEQL:
        {
            cout << ipx++ << " DNOTEQL" << endl;
            break;
        }
        }
    }

}


void Interpreter::exec()
{
    ipx = IR.size() - 4;
    while(ipx < IR.size())
    {
        switch(IR[ipx].opcode)
        {
        case PUSHEBP:
        {
            data.address = ebp;
            memory.push_back(data);
            ipx++;
            break;
        }
        case NEWEBP:
        {
            ebp = memory.size() - 1;
            ipx++;
            break;
        }
        case MALLOC:
        {
            memory.push_back(data);
            ipx++;
            break;
        }        
        case MALLOCARR:
        {
            memory.resize(memory.size() + IR[ipx+1].opcode);
            ipx += 2;
            break;
        }
        case DTOI:
        {
            stk.top().iValue = (int)stk.top().dValue;
            ipx++;
            break;
        }
        case ITOD:
        {
            stk.top().dValue = (double)stk.top().iValue;
            ipx++;
            break;
        }
        case IGSTORE:
        {
            memory[IR[ipx+1].opcode].iValue = stk.top().iValue;
            stk.pop();
            ipx += 2;
            break;
        }        
        case ILSTORE:
        {
            memory[IR[ipx+1].iValue+ebp].iValue = stk.top().iValue;
            stk.pop();
            ipx += 2;
            break;
        }
        case DGSTORE:
        {
            memory[IR[ipx+1].opcode].dValue = stk.top().dValue;
            stk.pop();
            ipx += 2;
            break;
        }
        case DLSTORE:
        {
            memory[IR[ipx+1].iValue+ebp].dValue = stk.top().dValue;
            stk.pop();
            ipx += 2;        
            break;
        }
        case IFCMP:
        {
            if(stk.top().iValue != 0)
                ipx = IR[ipx+1].opcode;
            else
                ipx += 2;
            stk.pop();
            break;
        }        
        case IFNCMP:
        {
            if(stk.top().iValue == 0)
                ipx = IR[ipx+1].opcode;
            else
                ipx += 2;
            stk.pop();
            break;
        }
        case JUMP:
        {
            ipx = IR[ipx+1].opcode;
            break;
        }
        case JUMPBACK:
        {
            ipx = memory[ebp-1].address;
            break;
        }
        case IRETURN:
        {
            irx = stk.top().iValue;
            stk.pop();
            ipx++;
            break;
        }        
        case DRETURN:
        {
            drx = stk.top().dValue;
            stk.pop();
            ipx++;
            break;
        }
        case ISTORETOP:
        {
            memory[memory.size()-1].iValue = stk.top().iValue;
            stk.pop();
            ipx++;
            break;
        }
        case DSTORETOP:
        {
            memory[memory.size()-1].dValue = stk.top().dValue;
            stk.pop();
            ipx++;
            break;
        }
        case PUSHADRS:
        {
            esp = memory.size();
            data.address = IR[ipx+1].opcode;
            memory.push_back(data);
            ipx += 2;
            break;
        }        
        case OLDEBP:
        {
            ebp = memory[ebp].address;
            ipx++;
            break;
        }
        case OLDESP:
        {
            memory.resize(esp-IR[ipx+1].opcode);
            ipx += 2;
            break;
        }
        case IRESULT:
        {
            data.iValue = irx;
            stk.push(data);
            ipx++;
            break;
        }
        case INEGATIVE:
        {
            stk.top().iValue = -stk.top().iValue;
            ipx++;
            break;
        }        
        case DRESULT:
        {
            data.dValue = drx;
            stk.push(data);
            ipx++;
            break;
        }
        case DNEGATIVE:
        {
            stk.top().dValue = -stk.top().dValue;
            ipx++;
            break;
        }
        case IGLOAD:
        {
            data.iValue = memory[IR[ipx+1].opcode].iValue;
            stk.push(data);
            ipx += 2;
            break;
        }
        case ILLOAD:
        {
            data.iValue = memory[IR[ipx+1].iValue+ebp].iValue;
            stk.push(data);
            ipx += 2;
            break;
        }        
        case DGLOAD:
        {
            data.dValue = memory[IR[ipx+1].opcode].dValue;
            stk.push(data);
            ipx += 2;
            break;
        }
        case DLLOAD:
        {
            data.dValue = memory[IR[ipx+1].iValue+ebp].dValue;
            stk.push(data);
            ipx += 2;
            break;
        }
        case ICONST:
        {
            data.iValue = IR[ipx+1].iValue;
            stk.push(data);
            ipx += 2;
            break;
        }
        case DCONST:
        {
            data.dValue = IR[ipx+1].dValue;
            stk.push(data);
            ipx += 2;
            break;
        }        
        case ITOD2:
        {
            MemType tmp = stk.top();
            stk.pop();
            stk.top().dValue = (double)stk.top().iValue;
            stk.push(tmp);
            ipx += 2;
            break;
        }
        case IADD:
        {
            int right = stk.top().iValue;
            stk.pop();
            int left = stk.top().iValue;
            stk.pop();
            data.iValue = left + right;
            stk.push(data);
            ipx++;
            break;
        }
        case DADD:
        {
            double right = stk.top().dValue;
            stk.pop();
            double left = stk.top().dValue;
            stk.pop();
            data.dValue = left + right;
            stk.push(data);    
            ipx++;  
            break;
        }
        case ISUB:
        {
            int right = stk.top().iValue;
            stk.pop();
            int left = stk.top().iValue;
            stk.pop();
            data.iValue = left - right;
            stk.push(data);
            ipx++;
            break;
        }        
        case DSUB:
        {
            double right = stk.top().dValue;
            stk.pop();
            double left = stk.top().dValue;
            stk.pop();
            data.dValue = left - right;
            stk.push(data); 
            ipx++;
            break;
        }
        case IMUL:
        {
            int right = stk.top().iValue;
            stk.pop();
            int left = stk.top().iValue;
            stk.pop();
            data.iValue = left * right;
            stk.push(data);
            ipx++;
            break;
        }
        case DMUL:
        {
            double right = stk.top().dValue;
            stk.pop();
            double left = stk.top().dValue;
            stk.pop();
            data.dValue = left * right;
            stk.push(data);  
            ipx++;
            break;
        }
        case IDIV:
        {
            int right = stk.top().iValue;
            stk.pop();
            int left = stk.top().iValue;
            stk.pop();
            data.iValue = left / right;
            stk.push(data);
            ipx++;
            break;
        }        
        case DDIV:
        {
            double right = stk.top().dValue;
            stk.pop();
            double left = stk.top().dValue;
            stk.pop();
            data.dValue = left / right;
            stk.push(data);  
            ipx++;
            break;
        }
        case ISML:
        {
            int right = stk.top().iValue;
            stk.pop();
            int left = stk.top().iValue;
            stk.pop();
            data.iValue = left < right;
            stk.push(data);
            ipx++;
            break;
        }
        case DSML:
        {
            double right = stk.top().dValue;
            stk.pop();
            double left = stk.top().dValue;
            stk.pop();
            data.dValue = left < right;
            stk.push(data);  
            ipx++;
            break;
        }
        case IBIG:
        {
            int right = stk.top().iValue;
            stk.pop();
            int left = stk.top().iValue;
            stk.pop();
            data.iValue = left > right;
            stk.push(data);
            ipx++;
            break;
        }        
        case DBIG:
        {
            double right = stk.top().dValue;
            stk.pop();
            double left = stk.top().dValue;
            stk.pop();
            data.dValue = left > right;
            stk.push(data);  
            ipx++;
            break;
        }
        case ISMLEQL:
        {
            int right = stk.top().iValue;
            stk.pop();
            int left = stk.top().iValue;
            stk.pop();
            data.iValue = left <= right;
            stk.push(data);  
            ipx++;     
            break;
        }
        case DSMLEQL:
        {
            double right = stk.top().dValue;
            stk.pop();
            double left = stk.top().dValue;
            stk.pop();
            data.dValue = left <= right;
            stk.push(data);  
            ipx++;
            break;
        }        
        case IEQLEQL:
        {
            int right = stk.top().iValue;
            stk.pop();
            int left = stk.top().iValue;
            stk.pop();
            data.iValue = left == right;
            stk.push(data);
            ipx++;
            break;
        }
        case DEQLEQL:
        {
            double right = stk.top().dValue;
            stk.pop();
            double left = stk.top().dValue;
            stk.pop();
            data.dValue = left == right;
            stk.push(data);  
            ipx++;
            break;
        }
        case IBIGEQL:
        {
            int right = stk.top().iValue;
            stk.pop();
            int left = stk.top().iValue;
            stk.pop();
            data.iValue = left >= right;
            stk.push(data);
            ipx++;
            break;
        }
        case DBIGEQL:
        {
            double right = stk.top().dValue;
            stk.pop();
            double left = stk.top().dValue;
            stk.pop();
            data.dValue = left >= right;
            stk.push(data);  
            ipx++;
            break;
        }        
        case INOTEQL:
        {
            int right = stk.top().iValue;
            stk.pop();
            int left = stk.top().iValue;
            stk.pop();
            data.iValue = left != right;
            stk.push(data);
            ipx++;
            break;
        }
        case DNOTEQL:
        {
            double right = stk.top().dValue;
            stk.pop();
            double left = stk.top().dValue;
            stk.pop();
            data.dValue = left != right;
            stk.push(data);  
            ipx++;
            break;
        }
        }
    }

}


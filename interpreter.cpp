#include "interpreter.h"



void Interpreter::interpret(AST& tree)
{
    root = tree.getRoot();
    translateIR();
    exec();
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
                int varType = static_cast<VarNode*>(no)->varType;
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
            size_t name = static_cast<AssignNode*>(n)->name;
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
                IR[breakpoints.back].opcode = IR.size();
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
    instruction.opcode = space + 1;
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
            std::pair<int, bool> result = addressTable.find(static_cast<VarValNode*>(n)->name)->second;
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


void Interpreter::exec()
{
    using namespace std;

    ebp = memory.size();
    while(ipx < IR.size())
    {
        if(IR[ipx].opcode == PUSHEBP)
        {
            cout << ipx++ << " PUSHEBP" << endl;
        }
        else if(IR[ipx].opcode == NEWEBP)
        {
            cout << ipx++ << " NEWEBP" << endl;
        }
        else if(IR[ipx].opcode == MALLOC)
        {
            cout << ipx++ << " MALLOC" << endl;
        }        
        else if(IR[ipx].opcode == MALLOCARR)
        {
            cout << ipx++ << " MALLOCARR" << endl;
            cout << ipx++ << IR[ipx].opcode << endl;
        }
        else if(IR[ipx].opcode == DTOI)
        {
            cout << ipx++ << " DTOI" << endl;
        }
        else if(IR[ipx].opcode == ITOD)
        {
            cout << ipx++ << " ITOD" << endl;
        }
        else if(IR[ipx].opcode == IGSTORE)
        {
            cout << ipx++ << " IGSTORE" << endl;
            cout << ipx++ << IR[ipx].opcode << endl;
        }        
        else if(IR[ipx].opcode == ILSTORE)
        {
            cout << ipx++ << " ILSTORE" << endl;
            cout << ipx++ << IR[ipx].iValue << endl;
        }
        else if(IR[ipx].opcode == DGSTORE)
        {
            cout << ipx++ << " DGSTORE" << endl;
            cout << ipx++ << IR[ipx].opcode << endl;
        }
        else if(IR[ipx].opcode == DLSTORE)
        {
            cout << ipx++ << " DLSTORE" << endl;
            cout << ipx++ << IR[ipx].iValue << endl;            
        }
        else if(IR[ipx].opcode == IFCMP)
        {
            cout << ipx++ << " IFCMP" << endl;
            cout << ipx++ << IR[ipx].opcode << endl;
        }        
        else if(IR[ipx].opcode == IFNCMP)
        {
            cout << ipx++ << " IFNCMP" << endl;
            cout << ipx++ << IR[ipx].opcode << endl;
        }
        else if(IR[ipx].opcode == JUMP)
        {
            cout << ipx++ << " JUMP" << endl;
            cout << ipx++ << IR[ipx].opcode << endl;
        }
        else if(IR[ipx].opcode == JUMPBACK)
        {
            cout << ipx++ << " JUMPBACK" << endl;
        }
        else if(IR[ipx].opcode == IRETURN)
        {
            cout << ipx++ << " IRETURN" << endl;
        }        
        else if(IR[ipx].opcode == DRETURN)
        {
            cout << ipx++ << " DRETURN" << endl;
        }
        else if(IR[ipx].opcode == ISTORETOP)
        {
            cout << ipx++ << " ISTORETOP" << endl;
        }
        else if(IR[ipx].opcode == DSTORETOP)
        {
            cout << ipx++ << " DSTORETOP" << endl;
        }
        else if(IR[ipx].opcode == PUSHADRS)
        {
            cout << ipx++ << " PUSHADRS" << endl;
        }        
        else if(IR[ipx].opcode == OLDEBP)
        {
            cout << ipx++ << " OLDEBP" << endl;
        }
        else if(IR[ipx].opcode == OLDESP)
        {
            cout << ipx++ << " OLDESP" << endl;
        }
        else if(IR[ipx].opcode == IRESULT)
        {
            cout << ipx++ << " IRESULT" << endl;        
        }
        else if(IR[ipx].opcode == INEGATIVE)
        {
            cout << ipx++ << " INEGATIVE" << endl;
        }        
        else if(IR[ipx].opcode == DRESULT)
        {
            cout << ipx++ << " DRESULT" << endl;
        }
        else if(IR[ipx].opcode == DNEGATIVE)
        {
            cout << ipx++ << " DNEGATIVE" << endl;
        }
        else if(IR[ipx].opcode == IGLOAD)
        {
            cout << ipx++ << " IGLOAD" << endl;
            cout << ipx++ << IR[ipx].opcode << endl;
        }
        else if(IR[ipx].opcode == ILLOAD)
        {
            cout << ipx++ << " ILLOAD" << endl;
            cout << ipx++ << IR[ipx].iValue << endl;
        }        
        else if(IR[ipx].opcode == DGLOAD)
        {
            cout << ipx++ << " DGLOAD" << endl;
            cout << ipx++ << IR[ipx].opcode << endl;
        }
        else if(IR[ipx].opcode == DLLOAD)
        {
            cout << ipx++ << " DLLOAD" << endl;
            cout << ipx++ << IR[ipx].iValue << endl;
        }
        else if(IR[ipx].opcode == ICONST)
        {
            cout << ipx++ << " ICONST" << endl;
            cout << ipx++ << IR[ipx].iValue << endl;
        }
        else if(IR[ipx].opcode == DCONST)
        {
            cout << ipx++ << " DCONST" << endl;
            cout << ipx++ << IR[ipx].dValue << endl;
        }        
        else if(IR[ipx].opcode == ITOD2)
        {
            cout << ipx++ << " ITOD2" << endl;
        }
        else if(IR[ipx].opcode == IADD)
        {
            cout << ipx++ << " IADD" << endl;
        }
        else if(IR[ipx].opcode == DADD)
        {
            cout << ipx++ << " DADD" << endl;        
        }
        else if(IR[ipx].opcode == ISUB)
        {
            cout << ipx++ << " ISUB" << endl;
        }        
        else if(IR[ipx].opcode == DSUB)
        {
            cout << ipx++ << " DSUB" << endl;
        }
        else if(IR[ipx].opcode == IMUL)
        {
            cout << ipx++ << " IMUL" << endl;
        }
        else if(IR[ipx].opcode == DMUL)
        {
            cout << ipx++ << " DMUL" << endl;
        }
        else if(IR[ipx].opcode == IDIV)
        {
            cout << ipx++ << " IDIV" << endl;
        }        
        else if(IR[ipx].opcode == DDIV)
        {
            cout << ipx++ << " DDIV" << endl;
        }
        else if(IR[ipx].opcode == ISML)
        {
            cout << ipx++ << " ISML" << endl;
        }
        else if(IR[ipx].opcode == DSML)
        {
            cout << ipx++ << " DSML" << endl;
        }
        else if(IR[ipx].opcode == IBIG)
        {
            cout << ipx++ << " IBIG" << endl;
        }        
        else if(IR[ipx].opcode == DBIG)
        {
            cout << ipx++ << " DBIG" << endl;
        }
        else if(IR[ipx].opcode == ISMLEQL)
        {
            cout << ipx++ << " ISMLEQL" << endl;        
        }
        else if(IR[ipx].opcode == DSMLEQL)
        {
            cout << ipx++ << " DSMLEQL" << endl;
        }        
        else if(IR[ipx].opcode == IEQLEQL)
        {
            cout << ipx++ << " IEQLEQL" << endl;
        }
        else if(IR[ipx].opcode == DEQLEQL)
        {
            cout << ipx++ << " DEQLEQL" << endl;
        }
        else if(IR[ipx].opcode == IBIGEQL)
        {
            cout << ipx++ << " IBIGEQL" << endl;
        }
        else if(IR[ipx].opcode == DBIGEQL)
        {
            cout << ipx++ << " DBIGEQL" << endl;
        }        
        else if(IR[ipx].opcode == INOTEQL)
        {
            cout << ipx++ << " INOTEQL" << endl;
        }
        else if(IR[ipx].opcode == DNOTEQL)
        {
            cout << ipx++ << " DNOTEQL" << endl;
        }
    }

}

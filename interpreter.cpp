#include "interpreter.h"



int Interpreter::exec(AST& tree)
{
    root = tree.getRoot();
    translateIR();
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
            size_t name = static_cast<VarNode*>(n)->name;
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
    }
}

void Interpreter::translateCall(ASTNode* n)
{
    int space = 0;
    for(ASTNode* node=static_cast<CallNode*>(n)->args; node!=NULL; node=node->next)
    {
        int typeFlag = static_cast<ExprNode*>(node)->varType;
        translateExpr(node);
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
        }
        else if(node->nodeType == VARVALNODE)
            ;
    }
}


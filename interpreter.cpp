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
            instruction.opcode = PUSHEBP;
            IR.push_back(instruction);
            instruction.opcode = MODEBP;
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
        {
            for(ASTNode* node=static_cast<CallNode*>(n)->args; node!=NULL; node=node->next)
            {
                int typeFlag = static_cast<ExprNode*>(node)->varType;
                translateExpr(node);
                instruction.opcode = MALLOC;
                IR.push_back(instruction);
                if(typeFlag == TINT)
                    instruction.opcode = ISTORE;
                else
                    instruction.opcode = DSTORE;
                IR.push_back(instruction);
                instruction.opcode = local;
                IR.push_back(instruction);
                local++;
            }
            instruction.opcode = PUSHADRS;
            IR.push_back(instruction);
            instruction.opcode = IR.size();
            IR.push_back(instruction);
            


        }
    }

}

void Interpreter::translateExpr(ASTNode* n)
{
    for(ASTNode* node=static_cast<ExprNode*>(n)->value; node!=NULL; node=node->next)
    {

    }
}


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
                memory;
            }
            else
                return;
        }
    }
}


void AST::test()
{
    enum { IALLOC, DALLOC, IAALLOC, DAALLOC };
    union IRType
    {
        size_t opcode;
        double dValue;
        int iValue;
    } unit;
    std::map<size_t, std::pair<int, bool> > addressTable;
    std::vector<IRType> IR;
    ASTNode* node = root;

    while(node != NULL)
    {
        if(node->nodeType == VARNODE)
        {
            int vt = static_cast<VarNode*>(node)->varType;
            size_t name = static_cast<VarNode*>(node)->name;
            addressTable.insert(std::pair<size_t, std::pair<int, bool> >(name, std::pair<int, bool>(IR[0].iValue, true)));
        }
        else if(node->nodeType == FUNCNODE)
        {
            IR.push_back(PUSHEBP);
            IR.push_back(CHANGEEBP);
            for(ASTNode* n=static_cast<FuncNode*>(node)->lclStmts; n!=NULL; n=n->next)
            {
                if(n->nodeType == VARNODE)
                {
                    if()
                }


            }


        }
        else
            break;
    }


    
}
#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_

#include "AST.h"


class Interpreter
{
private:
    enum { VARNODE=1, FUNCNODE, CALLNODE, ASSIGNNODE, IFNODE, WHILENODE, BREAKNODE, RETURNNODE, EXPRNODE, INTNODE, DOUBLENODE, VARVALNODE, OPERATORNODE };
    enum { TVOID=0, TINT, TDOUBLE, TINTARR, TDOUBLEARR, TFUNCVOID, TFUNCINT, TFUNCDOUBLE };
    //enum { IALLOC, DALLOC, IAALLOC, DAALLOC };
    ASTNode* root;
    union IRType
    {
        size_t opcode;
        double dValue;
        int iValue;
    } instruction;
    std::map<size_t, std::pair<int, bool> > addressTable;
    std::vector<IRType> IR;
    size_t globalAddress;

    union MemType
    {
        double dValue;
        int iValue;
    } data;
    std::vector<MemType> memory;




    void translateIR();
    void translateLclStmts(ASTNode* n, int& local);
    void translateCall(ASTNode* n);
    void translateExpr(ASTNode* n);


public:
    Interpreter() {}
    int exec(AST& tree);
    ~Interpreter() {}
};




















#endif
#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_

#include "AST.h"


class Interpreter
{
private:
    enum { VARNODE=1, FUNCNODE, CALLNODE, ASSIGNNODE, IFNODE, WHILENODE, BREAKNODE, RETURNNODE, EXPRNODE, INTNODE, DOUBLENODE, VARVALNODE, OPERATORNODE };
    enum { TVOID=0, TINT, TDOUBLE, TINTARR, TDOUBLEARR, TFUNCVOID, TFUNCINT, TFUNCDOUBLE };
    enum { PUSHEBP, NEWEBP, MALLOC, MALLOCARR, DTOI, ITOD, IGSTORE, ILSTORE, DGSTORE, DLSTORE, IFCMP, IFNCMP, JUMP, JUMPBACK,
            IRETURN, DRETURN, ISTORETOP, DSTORETOP, PUSHADRS, OLDEBP, OLDESP, IRESULT, INEGATIVE, DRESULT, DNEGATIVE,
            IGLOAD, ILLOAD, DGLOAD, DLLOAD, ICONST, DCONST, ITOD2, IADD, DADD, ISUB, DSUB, IMUL, DMUL, IDIV, DDIV, 
            ISML, DSML, IBIG, DBIG, ISMLEQL, DSMLEQL, IEQLEQL, DEQLEQL, IBIGEQL, DBIGEQL, INOTEQL, DNOTEQL};
    ASTNode* root;
    union IRType
    {
        size_t opcode;
        double dValue;
        int iValue;
    } instruction;
    std::map<size_t, std::pair<int, bool> > addressTable;
    std::vector<IRType> IR;
    std::vector<size_t> breakpoints;
    std::stack<int> typeStack;

    union MemType
    {
        size_t address;
        double dValue;
        int iValue;
    } data;
    int irx;
    double drx;
    size_t ipx;
    size_t ebp;
    size_t esp;
    std::vector<MemType> memory;
    std::stack<MemType> stk;

    void translateIR();
    void translateLclStmts(ASTNode* n, int& local);
    void translateCall(ASTNode* n);
    void translateExpr(ASTNode* n);
    void exec();    
    void showIR();
public:
    Interpreter() : root(NULL), ipx(0), ebp(0), esp(0) {}
    void interpret(AST& tree);
    void showInterpretation(AST& tree);
    ~Interpreter() {}
};




















#endif
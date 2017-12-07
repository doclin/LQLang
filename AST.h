#ifndef _AST_H_
#define _AST_H_

















class AST
{
private:
    int i;
public:
    void addRoot();
    void addVariable(const char* name, size_t nameLength, int typeFlag, size_t arrLength=0);
    void addFuncDef(const char* name, size_t nameLength, int typeFlag, size_t arrLength=0);
    void endFuncDef();
    void addParameter(const char* name, size_t nameLength, int typeFlag, size_t arrLength=0);
    void addFuncCall(const char* name, size_t nameLength);
    void addIf();
    void addElse();
    void endElse();
    void endIf();
    void addWhile();
    void endWhile();
    void addBreak();
    void addReturn();
    void addAssign(const char* name, size_t nameLength, size_t index=0);

    void addExpr();
    void addExpr1();
    void addExpr2();
    void addExpr3();
};





#endif
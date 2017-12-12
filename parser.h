#ifndef _PARSER_H_
#define _PARSER_H_

#include <cstring>
#include "AST.h"

class Parser
{
private:
    enum { INTVALUE=2001, DOUBLEVALUE, INTK=3001, DOUBLEK, IFK, ELSEK, WHILEK, BREAKK, VOIDK, RETURNK, IDK };
    enum { TVOID=0, TINT, TDOUBLE, TINTARR, TDOUBLEARR, TFUNCVOID, TFUNCINT, TFUNCDOUBLE };
    const char* code;
    int crtToken;
    size_t line;
    size_t pos;
    double doubleValue;
    int integerValue;
    const char* IDValue;
    size_t IDLength;
    bool success;

    AST tree;
    
    int getNextToken();
    inline bool isBlank(char c)     { return (c==' ' || c=='\t' || c=='\r' || c=='\n'); }
    inline bool isSPChar(char c)    { return (c==';' || c=='[' || c==']' || c=='{' || c=='}' || c=='(' || c==')' || c==','); }
    inline bool isOPChar(char c)    { return (c=='+' || c=='-' || c=='*' || c=='/'); }
    inline bool isCPChar(char c)    { return (c=='<' || c=='>' || c=='='); }
    inline bool isNum(char c)   { return (c>='0' && c<='9'); }
    inline int getNum(char c)   { return (c - '0'); }    
    inline bool isABC(char c)   { return ((c>='a' && c<='z') || (c>='A' && c<='Z')); }
    int NumDFA();
    int IDDFA();
    inline bool isKeyWord(const char* s, const char* k, size_t len) { return (memcmp(s, k, len)==0 && !isNum(*(code+len)) && !isABC(*(code+len))); }

    void program();
    void glbStatements();
    void lclStatements();
    void varStmt();
    void ifStmt();
    void elseStmt();
    void whileStmt();
    void breakStmt();
    void returnStmt();
    void parameters();
    void arguments();
    void variables(int typeFlag, size_t arrLength=0);
    void expr(bool nf);
    void operand(bool nf);
    inline bool isOperator(int t) { return (t=='<' || t=='>' || t==1000+'=' || t==1000+'<' || t==1000+'!' || t==1000+'>' || t=='+' || t=='-' || t=='*' || t=='/'); }
public:
    Parser() : line(1), pos(1), success(true) {}
    ~Parser() {}
    AST& parse(const char* c);
    bool isSuccess() { return success; }
};





#endif
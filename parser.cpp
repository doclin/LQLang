#include "parser.h"
#include <iostream>

void Parser::parse(const char* c)
{
    code = c;
    crtToken = getNextToken();
    program();
}

void Parser::program()
{
    if(crtToken==INTK || crtToken==DOUBLEK || crtToken==VOIDK)
    {
        tree.addRoot();
        glbStatements();
    }
    else
        throw 0;
}

void Parser::glbStatements()
{
    if(crtToken==INTK || crtToken==DOUBLEK)
    {
        int typeFlag;
        size_t arrLength;
        //varType();


        
    }
}


void Parser::expr()
{
    if(crtToken==IDK || crtToken=='(' || crtToken=='-' || crtToken==INTVALUE || crtToken==DOUBLEVALUE)
    {
        tree.addExpr();
        expr1();
        while(isLevel1(crtToken))
        {
            //tree.addLevel1();
            crtToken = getNextToken();
            expr1();
        }
        if(crtToken==')' || crtToken==';' || crtToken==',')
            ;
        else
            throw 1;
    }
    else
        throw 0;
}

void Parser::expr1()
{
    if(crtToken==IDK || crtToken=='(' || crtToken=='-' || crtToken==INTVALUE || crtToken==DOUBLEVALUE)
    {
        //tree
        expr2();
        while(isLevel2(crtToken))
        {
            //tree
            crtToken = getNextToken();
            expr2();
        }
        if(crtToken==')' || crtToken==';' || crtToken==',' || isLevel1(crtToken))
            ;
        else
            throw 1;
    }
    else
        throw 1;
}

void Parser::expr2()
{
    if(crtToken==IDK || crtToken=='(' || crtToken=='-' || crtToken==INTVALUE || crtToken==DOUBLEVALUE)
    {
        //tree
        expr3();
        while(isLevel3(crtToken))
        {
            //tree
            crtToken = getNextToken();
            expr3();
        }
        if(crtToken==')' || crtToken==';' || crtToken==',' || isLevel1(crtToken) || isLevel2(crtToken))
            ;
        else
            throw 1;
    }
    else
        throw 1;    
}

void Parser::expr3()
{
    int negativeFlag = false;
    if(crtToken=='-')
    {
        negativeFlag = true;
        crtToken = getNextToken();
    }
    if(crtToken==IDK)
    {
        
    }
}


























int Parser::getNextToken()
{
    while(isBlank(*code))
    {
        if(*code == '\n')
            { line++; pos = 1; }
        else
            pos++;
        code++;
    }
    if(*code == '\0')
        return 0;
    else if((isCPChar(*code) || *code=='!') && *(code+1)=='=')
    {
        int token = *code + 1000;
        pos  += 2;
        code += 2;
        return token;
    }
    else if(isSPChar(*code) || isOPChar(*code) || isCPChar(*code))
    {
        pos++;
        return *code++;
    }
    else if(isNum(*code))
        return NumDFA();
    else if(isABC(*code))
        return IDDFA();
    else
        return -1;
}

int Parser::NumDFA()
{
    int num = 0;
    while(isNum(*code))
        num = 10 * num + getNum(*code++);
    if(*code == '.')
    {
        int count = 1;
        code++;
        while(isNum(*code))
        {
            num = 10 * num + getNum(*code++);
            count *= 10;
        }
        doubleValue = 1.0 * num / count;
        return DOUBLEVALUE;
    }
    else
    {
        integerValue = num;
        return INTVALUE;
    }  
}

int Parser::IDDFA()
{
    if(isKeyWord(code, "int", 3))
        { code+=3; return INTK; }
    else if(isKeyWord(code, "double", 6))
        { code+=6; return DOUBLEK; }
    else if(isKeyWord(code, "if", 2))
        { code+=2; return IFK; }
    else if(isKeyWord(code, "else", 4))
        { code+=4; return ELSEK; }
    else if(isKeyWord(code, "while", 5))
        { code+=5; return WHILEK; }
    else if(isKeyWord(code, "break", 5))
        { code+=5; return BREAKK; }
    else if(isKeyWord(code, "void", 4))
        { code+=4; return VOIDK; }
    else if(isKeyWord(code, "return", 6))
        { code+=6; return RETURNK; }
    else
    {
        IDValue = code;
        while(isABC(*code) || isNum(*code))
            code++;
        IDLength = code - IDValue;
        return IDK;
    }
}
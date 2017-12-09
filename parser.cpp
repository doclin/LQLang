#include "parser.h"
#include <iostream>

void Parser::parse(const char* c)
{
    code = c;
    crtToken = getNextToken();
    try{
        program();
    }
    catch(size_t i)
    {
        std::cout << "GG:" << i << std::endl;
    }
}

void Parser::program()
{
    if(crtToken==INTK || crtToken==DOUBLEK || crtToken==VOIDK)
    {
        tree.addRoot();
        glbStatements();
    }
    else
        throw line;
}

void Parser::glbStatements()
{
    if(crtToken==INTK || crtToken==DOUBLEK)
    {
        int typeFlag;
        size_t arrLength = 0;
        if(crtToken==INTK)
            typeFlag = TINT;
        else
            typeFlag = TDOUBLE;
        crtToken = getNextToken();
        if(crtToken == '[')
        {
            if(getNextToken()==INTVALUE && getNextToken()==']')
            {
                if(typeFlag==TINT)
                    typeFlag = TINTARR;
                else
                    typeFlag = TDOUBLEARR;
                arrLength = integerValue;
                crtToken = getNextToken();
                if(crtToken == IDK)
                    variables(typeFlag, arrLength);
                else
                    throw line;
                if(crtToken == ';')
                    crtToken = getNextToken();
                else
                    throw 1;
            }
            else
                throw line;
        }
        else if(crtToken == IDK)
        {
            crtToken = getNextToken();
            if(crtToken=='(')
            {
                tree.addFuncDef(IDValue, IDLength, typeFlag);
                crtToken = getNextToken();
                parameters();
                tree.endParameters();
                if(crtToken==')' && getNextToken()=='{')
                {
                    crtToken = getNextToken();
                    lclStatements();
                }
                else
                    throw line;
                if(crtToken == '}')
                {
                    tree.endFuncDef();
                    crtToken = getNextToken();
                }
                else
                    throw line;
            }
            else if(crtToken == ';')
            {
                tree.addVariable(IDValue, IDLength, typeFlag, arrLength);
                crtToken = getNextToken();
            }
            else if(crtToken == ',')
            {
                tree.addVariable(IDValue, IDLength, typeFlag, arrLength);
                crtToken = getNextToken();
                variables(typeFlag, arrLength);
                if(crtToken == ';')
                    crtToken = getNextToken();
                else
                    throw line;
            }
            else
                throw line;
        }
        else
            throw line;  
    }
    else if(crtToken == VOIDK)
    {
        if(getNextToken()==IDK && getNextToken()=='(')
        {
            tree.addFuncDef(IDValue, IDLength, TVOID);
            crtToken = getNextToken();
            parameters();
            tree.endParameters();
            if(crtToken == ')' && getNextToken()=='{')
            {
                crtToken = getNextToken();
                lclStatements();
                if(crtToken == '}')
                {
                    tree.endFuncDef();
                    crtToken = getNextToken();
                }     
                else
                    throw line;
            }
            else
                throw line;
        }
        else
            throw line;
    }
    else if(crtToken == 0)
        return;
    else
        throw line;
    glbStatements();
}

void Parser::lclStatements()
{
    if(crtToken==INTK || crtToken==DOUBLEK)
        varStmt();
    else if(crtToken == IFK)
        ifStmt();
    else if(crtToken == WHILEK)
        whileStmt();
    else if(crtToken == BREAKK)
        breakStmt();
    else if(crtToken == IDK)
    {
        crtToken = getNextToken();
        if(crtToken == '(')
        {
            tree.addFuncCall(IDValue, IDLength);
            crtToken = getNextToken();
            arguments();
            tree.endArguments();
            if(crtToken==')' && getNextToken()==';')
                crtToken = getNextToken();
            else
                throw line;
        }
        else if(crtToken == '[')
        {
            if(getNextToken()==INTVALUE && getNextToken()==']')
            {
                crtToken = getNextToken();
                if(crtToken == '=')
                {
                    tree.addAssign(IDValue, IDLength, integerValue);
                    crtToken = getNextToken();
                    tree.addExpr();
                    expr();
                    tree.endExpr();
                    if(crtToken == ';')
                    {
                        tree.endAssign();
                        crtToken = getNextToken();
                    }            
                    else
                        throw line;
                }
                else
                    throw line;
            }
            else
                throw line;
        }
        else if(crtToken == '=')
        {
            tree.addAssign(IDValue, IDLength);
            crtToken = getNextToken();
            tree.addExpr();
            expr();
            tree.endExpr();
            if(crtToken == ';')
            {
                tree.endAssign();
                crtToken = getNextToken();
            }  
            else
                throw line;
        }
        else
            throw line;
    }
    else if(crtToken == RETURNK)
        returnStmt();
    else if(crtToken == '}')
        return;
    else
        throw line;
    lclStatements();
}

void Parser::varStmt()
{
    if(crtToken==INTK || crtToken==DOUBLEK)
    {
        int typeFlag;
        size_t arrLength;
        if(crtToken==INTK)
            typeFlag = TINT;
        else
            typeFlag = TDOUBLE;
        crtToken = getNextToken();
        if(crtToken == '[')
        {
            if(getNextToken()==INTVALUE && getNextToken()==']')
            {
                if(typeFlag==TINT)
                    typeFlag = TINTARR;
                else
                    typeFlag = TDOUBLEARR;
                arrLength = INTVALUE;
                crtToken = getNextToken();
            }
            else
                throw line;
        }
        variables(typeFlag, arrLength);
        if(crtToken == ';')
            crtToken = getNextToken();
        else
            throw 1;
    }
    else
        throw line;
}

void Parser::ifStmt()
{
    if(crtToken == IFK)
    {
        if(getNextToken() == '(')
        {
            tree.addIf();
            crtToken = getNextToken();
            tree.addExpr();
            expr();
            tree.endExpr();
            tree.endIfVal();
            if(crtToken==')' && getNextToken()=='{')
            {
                crtToken = getNextToken();
                lclStatements();
                tree.endIfTrue();
                if(crtToken == '}')
                {
                    crtToken = getNextToken();
                    elseStmt();
                    tree.endIFFalse();
                }
                else
                    throw line;
            }
            else
                throw line;
        }
        else
            throw line;
    }
    else
        throw line;
}

void Parser::elseStmt()
{
    if(crtToken == ELSEK)
    {
        if(getNextToken() == '{')
        {
            crtToken = getNextToken();
            lclStatements();
            if(crtToken == '}')
            {
                crtToken = getNextToken();
            }
            else
                throw line;
        }
        else
            throw line;
    }
    else if(crtToken==INTK || crtToken==DOUBLEK || crtToken==IFK || crtToken==WHILEK || crtToken==BREAKK || crtToken==RETURNK || crtToken==IDK || crtToken=='}')
        return;
    else
        throw line;
}



void Parser::whileStmt()
{
    if(crtToken == WHILEK)
    {
        if(getNextToken() == '(')
        {
            tree.addWhile();
            crtToken = getNextToken();
            tree.addExpr();
            expr();
            tree.endExpr();
            tree.endWhileVal();
            if(crtToken==')' && getNextToken()=='{')
            {
                crtToken = getNextToken();
                lclStatements();
                if(crtToken == '}')
                {
                    tree.endWhile();
                    crtToken = getNextToken();
                }
                else
                    throw line;
            }
            else
                throw line;
        }
        else
            throw line;
    }
    else
        throw line;
}

void Parser::breakStmt()
{
    if(crtToken == BREAKK)
    {
        if(getNextToken() == ';')
        {
            tree.addBreak();
            crtToken = getNextToken();
        }         
        else
            throw line;
    }
    else
        throw line;
}

void Parser::returnStmt()
{
    if(crtToken == RETURNK)
    {
        tree.addReturn();
        crtToken = getNextToken();
        tree.addExpr();
        expr();
        tree.endExpr();
        if(crtToken == ';')
        {
            tree.endReturn();
            crtToken = getNextToken();
        }
        else
            throw line;
    }
    else
        throw line;
}

void Parser::parameters()
{
    if(crtToken==INTK || crtToken==DOUBLEK)
    {
        int typeFlag;
        size_t arrLength;
        if(crtToken==INTK)
            typeFlag = TINT;
        else
            typeFlag = TDOUBLE;
        crtToken = getNextToken();
        if(crtToken=='[')
        {
            if(getNextToken()==INTVALUE && getNextToken()==']')
            {
                if(typeFlag==TINT)
                    typeFlag = TINTARR;
                else
                    typeFlag = TDOUBLEARR;
                arrLength = INTVALUE;
                crtToken = getNextToken();
            }
            else
                throw line;
        }
        if(crtToken == IDK)
        {
            tree.addVariable(IDValue, IDLength, typeFlag, arrLength);
            crtToken = getNextToken();
            if(crtToken == ')')
                return;
            else if(crtToken == ',')
            {
                crtToken = getNextToken();
                parameters();
            }
            else
                throw line;
        }
        else
            throw line;
    }
    else if(crtToken == ')')
        return;
    else
        throw line;    
}

void Parser::arguments()
{
    if(crtToken==IDK || crtToken=='(' || crtToken=='-' || crtToken==INTVALUE || crtToken==DOUBLEVALUE)
    {
        tree.addExpr();
        expr();
        tree.endExpr();
        if(crtToken == ',')
        {
            crtToken = getNextToken();
            arguments();
        }
        else if(crtToken == ')')
        {
            return;
        }
        else
            throw line;
    }
    else if(crtToken == ')')
        return;
    else
        throw line;
}

void Parser::variables(int typeFlag, size_t arrLength)
{
    if(crtToken == IDK)
    {
        tree.addVariable(IDValue, IDLength, typeFlag, arrLength);
        crtToken = getNextToken();
        if(crtToken == ',')
        {
            crtToken = getNextToken();
            variables(typeFlag, arrLength);
        }
        else if(crtToken == ';')
            return;
        else
            throw line;
    }
    else if(crtToken == ';')
        return;
    else
        throw line;
}

void Parser::expr()
{
    if(crtToken==IDK || crtToken=='(' || crtToken=='-' || crtToken==INTVALUE || crtToken==DOUBLEVALUE)
    {
        operand();
        if(isOperator(crtToken))
        {
            tree.addOperator(crtToken);
            crtToken = getNextToken();
            expr();
        }
        else if(crtToken==')' || crtToken==';' || crtToken==',')
            return;
        else
            throw line;
    }
    else
        throw line;
}

void Parser::operand()
{
    int negativeFlag = false;
    if(crtToken == '-')
    {
        negativeFlag = true;
        crtToken = getNextToken();
    }
    if(crtToken == IDK)
    {
        crtToken = getNextToken();
        if(crtToken == '(')
        {
            tree.addFuncCall(IDValue, IDLength, negativeFlag);
            crtToken = getNextToken();
            arguments();
            tree.endArguments();
            if(crtToken==')')
                crtToken = getNextToken();
            else
                throw line;
        }
        else if(crtToken == '[')
        {
            if(getNextToken()==INTVALUE && getNextToken()==']')
            {
                tree.addVarVal(IDValue, IDLength, integerValue, negativeFlag);
                crtToken = getNextToken();
            }
            else
                throw line;
        }
        else if(isOperator(crtToken) || crtToken==')' || crtToken==';' || crtToken==',')
        {
            tree.addVarVal(IDValue, IDLength, -1, negativeFlag);
            return;
        }
        else
            throw line;
    }
    else if(crtToken == '(')
    {
        tree.addOperator('(');
        crtToken = getNextToken();
        expr();
        if(crtToken == ')')
        {
            tree.addOperator(')');
            crtToken = getNextToken();
        }
        else
            throw line;
    }
    else if(crtToken==INTVALUE)
    {
        tree.addInt(integerValue, negativeFlag);
        crtToken = getNextToken();
    }
    else if(crtToken==DOUBLEVALUE)
    {
        tree.addDouble(doubleValue, negativeFlag);
        crtToken = getNextToken();
    }    
    else
        throw line;
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
#include "parser.h"
#include <iostream>

AST& Parser::parse(const char* c)
{
    code = c;
    if(code == NULL)
    {
        success = false;
        return tree;
    }
    crtToken = getNextToken();
    try{
        program();
    }
    catch(size_t i)
    {
        std::cout << "Error at Line:" << line << " Position: " << pos << std::endl;
        success = false;
    }
    return tree;
}

void Parser::program()
{
    if(crtToken==INTK || crtToken==DOUBLEK || crtToken==VOIDK)
    {
        if(!tree.addRoot())
            throw line;
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
                if(!tree.addFuncDef(IDValue, IDLength, typeFlag))
                    throw line;
                crtToken = getNextToken();
                parameters();
                if(!tree.endParameters())
                    throw line;
                if(crtToken==')' && getNextToken()=='{')
                {
                    crtToken = getNextToken();
                    lclStatements();
                }
                else
                    throw line;
                if(crtToken == '}')
                {
                    if(!tree.endFuncDef())
                        throw line;
                    crtToken = getNextToken();
                }
                else
                    throw line;
            }
            else if(crtToken == ';')
            {
                if(!tree.addVariable(IDValue, IDLength, typeFlag, arrLength))
                    throw line;
                crtToken = getNextToken();
            }
            else if(crtToken == ',')
            {
                if(!tree.addVariable(IDValue, IDLength, typeFlag, arrLength))
                    throw line;
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
            if(!tree.addFuncDef(IDValue, IDLength, TVOID))
                throw line;
            crtToken = getNextToken();
            parameters();
            if(!tree.endParameters())
                throw line;
            if(crtToken == ')' && getNextToken()=='{')
            {
                crtToken = getNextToken();
                lclStatements();
                if(crtToken == '}')
                {
                    if(!tree.endFuncDef())
                        throw line;
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
            if(!tree.addFuncCall(IDValue, IDLength))
                throw line;
            crtToken = getNextToken();
            arguments();
            if(!tree.endArguments())
                throw line;
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
                    if(!tree.addAssign(IDValue, IDLength, integerValue))
                        throw line;
                    crtToken = getNextToken();
                    if(!tree.addExpr())
                        throw line;
                    expr(false);
                    if(!tree.endExpr())
                        throw line;
                    if(crtToken == ';')
                    {
                        if(!tree.endAssign())
                            throw line;
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
            if(!tree.addAssign(IDValue, IDLength))
                throw line;
            crtToken = getNextToken();
            if(!tree.addExpr())
                throw line;
            expr(false);
            if(!tree.endExpr())
                throw line;
            if(crtToken == ';')
            {
                if(!tree.endAssign())
                    throw line;
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
            if(!tree.addIf())
                throw line;
            crtToken = getNextToken();
            if(!tree.addExpr())
                throw line;
            expr(false);
            if(!tree.endExpr())
                throw line;
            if(!tree.endIfVal())
                throw line;
            if(crtToken==')' && getNextToken()=='{')
            {
                crtToken = getNextToken();
                lclStatements();
                if(!tree.endIfTrue())
                    throw line;
                if(crtToken == '}')
                {
                    crtToken = getNextToken();
                    elseStmt();
                    if(!tree.endIFFalse())
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
            if(!tree.addWhile())
                throw line;
            crtToken = getNextToken();
            if(!tree.addExpr())
                throw line;
            expr(false);
            if(!tree.endExpr())
                throw line;
            if(!tree.endWhileVal())
                throw line;
            if(crtToken==')' && getNextToken()=='{')
            {
                crtToken = getNextToken();
                lclStatements();
                if(crtToken == '}')
                {
                    if(!tree.endWhile())
                        throw line;
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
            if(!tree.addBreak())
                throw line;
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
        if(!tree.addReturn())
            throw line;
        crtToken = getNextToken();
        if(!tree.addExpr())
            throw line;
        expr(false);
        if(!tree.endExpr())
            throw line;
        if(crtToken == ';')
        {
            if(!tree.endReturn())
                throw line;
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
        size_t arrLength = 0;
        if(crtToken==INTK)
            typeFlag = TINT;
        else
            typeFlag = TDOUBLE;
        crtToken = getNextToken();
        if(crtToken == IDK)
        {
            if(!tree.addVariable(IDValue, IDLength, typeFlag, arrLength))
                throw line;
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
        if(!tree.addExpr())
            throw line;
        expr(false);
        if(!tree.endExpr())
            throw line;
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
        if(!tree.addVariable(IDValue, IDLength, typeFlag, arrLength))
            throw line;
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

void Parser::expr(bool nf)
{
    if(crtToken==IDK || crtToken=='(' || crtToken=='-' || crtToken==INTVALUE || crtToken==DOUBLEVALUE)
    {
        operand(nf);
        if(isOperator(crtToken))
        {
            if(!tree.addOperator(crtToken))
                throw line;
            crtToken = getNextToken();
            expr(nf);
        }
        else if(crtToken==')' || crtToken==';' || crtToken==',')
            return;
        else
            throw line;
    }
    else
        throw line;
}

void Parser::operand(bool nf)
{
    if(crtToken == '-')
    {
        nf = !nf;
        crtToken = getNextToken();
    }
    if(crtToken == IDK)
    {
        crtToken = getNextToken();
        if(crtToken == '(')
        {
            if(!tree.addFuncCall(IDValue, IDLength, nf))
                throw line;
            crtToken = getNextToken();
            arguments();
            if(!tree.endArguments())
                throw line;
            if(crtToken==')')
                crtToken = getNextToken();
            else
                throw line;
        }
        else if(crtToken == '[')
        {
            if(getNextToken()==INTVALUE && getNextToken()==']')
            {
                if(!tree.addVarVal(IDValue, IDLength, integerValue, nf))
                    throw line;
                crtToken = getNextToken();
            }
            else
                throw line;
        }
        else if(isOperator(crtToken) || crtToken==')' || crtToken==';' || crtToken==',')
        {
            if(!tree.addVarVal(IDValue, IDLength, -1, nf))
                throw line;
            return;
        }
        else
            throw line;
    }
    else if(crtToken == '(')
    {
        if(!tree.addOperator('('))
            throw line;
        crtToken = getNextToken();
        expr(nf);
        if(crtToken == ')')
        {
            if(!tree.addOperator(')'))
                throw line;
            crtToken = getNextToken();
        }
        else
            throw line;
    }
    else if(crtToken==INTVALUE)
    {
        if(!tree.addInt(integerValue, nf))
            throw line;
        crtToken = getNextToken();
    }
    else if(crtToken==DOUBLEVALUE)
    {
        if(!tree.addDouble(doubleValue, nf))
            throw line;
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
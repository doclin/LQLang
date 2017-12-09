#include "symbolTable.h"



bool SymbolTable::addName(const char* name, size_t nameLength, size_t ID, int typeFlag)
{
    if(table.size() == 0)
        return false;
    std::pair<std::map<std::string, std::pair<size_t, int> >::iterator, bool> ret;
    ret = table.back().insert(std::pair<std::string, std::pair<size_t, int> >(std::string(name, nameLength), std::pair<size_t, int>(ID, typeFlag)));
    return ret.second;
}

bool SymbolTable::addScope()
{
    table.push_back(std::map<std::string, std::pair<size_t, int> >());
    return true;
}

bool SymbolTable::findName(const char* name, size_t nameLength, size_t& ID, int& typeFlag)
{
    std::vector< std::map<std::string, std::pair<size_t, int> > >::reverse_iterator rit = table.rbegin();
    std::map<std::string, std::pair<size_t, int> >::iterator ret;
    for(; rit != table.rend(); ++rit)
    {
        ret = (*rit).find(std::string(name, nameLength));
        if(ret == (*rit).end())
            continue;
        else
        {
            ID = ret->second.first;
            typeFlag = ret->second.second;
            return true;
        }
    }
    return false;
}

bool SymbolTable::popScope()
{
    if(table.size() == 0)
        return false;
    table.pop_back();
    return true;
}
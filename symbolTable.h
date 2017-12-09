#ifndef _SYMBOLTABLE_H_
#define _SYMBOLTABLE_H_

#include <vector>
#include <map>
#include <string>



class SymbolTable
{
private:
    std::vector< std::map<std::string, std::pair<size_t, int> > > table;
public:
    SymbolTable() {}
    bool addName(const char* name, size_t nameLength, size_t ID, int typeFlag);
    bool addScope();
    bool findName(const char* name, size_t nameLength, size_t& ID, int& typeFlag);
    bool popScope();
    ~SymbolTable() {}
};







#endif
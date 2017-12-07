#include <iostream>
#include "parser.h"
#include <vector>

using namespace std;

class Base {};
class Son : public Base
{
private:
    int a;
};



int main()
{
    vector<Base> v;
    Son s;
    v.push_back(s);


}
#include <iostream>
#include "symbolTable.h"

using namespace std;

int main()
{
    const char* s = "abcdefg higklmn opqrst uvwxyz";
    SymbolTable t;
    t.addScope();
    t.addName(s, 7, 1, 1);
    t.addName(s+8, 7, 2, 2);
    t.addScope();
    t.addName(s+16, 6, 3, 3);
    size_t a, b, c;
    int e, f, g;
    cout << t.findName(s+16, 6, a, e) << endl;
    cout << t.findName(s+16, 3, a, e) << endl;
    cout << t.findName(s+14, 6, a, e) << endl;
    cout << t.findName(s+8, 7, b, f) << endl;
    t.popScope();
    cout << t.findName(s+16, 6, a, e) << endl;
    cout << t.findName(s, 7, c, g) << endl;
    t.popScope();
    cout << a << b << c << endl;
    cout << e << f << g << endl;

    return 0;
}
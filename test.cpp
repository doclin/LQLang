#include <iostream>
#include "parser.h"

using namespace std;

int main()
{
    const char* s = "abcdefg";
    const char* tmp = s;
    test();
    s++;
    s++;
    cout << s - tmp << endl;

}

void test()
{
    int a;
}
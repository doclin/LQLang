#include <iostream>
#include "parser.h"

using namespace std;

int main()
{
    const char* s = "abcdefg";
    const char* tmp = s;
    s++;
    s++;
    cout << s - tmp << endl;

}
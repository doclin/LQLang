#include <iostream>
#include <fstream>
#include "parser.h"
#include "interpreter.h"

using namespace std;

int main () {
  std::ifstream is ("a.lq", std::ifstream::binary);
  if (is) {
    is.seekg (0, is.end);
    int length = is.tellg();
    is.seekg (0, is.beg);

    char * buffer = new char [length+1];

    is.read (buffer,length);

    is.close();
    buffer[length] = '\0';

    Parser p;
    AST& tree = p.parse(buffer);
    if(p.isSuccess())
    {
      Interpreter intprt;
      intprt.interpret(tree);
    }


    delete[] buffer;
  }
  return 0;
}
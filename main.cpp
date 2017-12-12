// read a file into memory
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include "parser.h"
#include "interpreter.h"

using namespace std;

int main () {

  std::ifstream is ("source.lq", std::ifstream::binary);
  if (is) {
    // get length of file:
    is.seekg (0, is.end);
    int length = is.tellg();
    is.seekg (0, is.beg);

    char * buffer = new char [length+1];

    //std::cout << "Reading " << length << " characters... ";
    // read data as a block:
    is.read (buffer,length);

    if (is)
      std::cout << "all characters read successfully.\n";
    else
      std::cout << "error: only " << is.gcount() << " could be read";
    is.close();
    buffer[length] = '\0';

    Parser p;
    AST& tree = p.parse(buffer);
    Interpreter intprt;
    intprt.interpret(tree);




    delete[] buffer;
  }
  return 0;
}
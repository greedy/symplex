#include "ast.hpp"
#include "parser.hpp"
#include "eval.hpp"
#include <fstream>
#include <string>
#include <iostream>

int main(int argc, char **argv)
{
  std::ifstream sourcefile(argv[1]);
  std::string sourcecode;
  std::getline(sourcefile, sourcecode, '\0');
  Parser p;
  p.parse(&sourcecode.front(), &sourcecode.back()+1);
  Runner r(p.result);
  r.runSuites();
  return 0;
}

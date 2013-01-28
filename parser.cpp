
#include <cstdlib>
#include <string>
#include <iostream>

#include "parser.hpp"

namespace SymplexLemon {
  void *Alloc( void* (*)(size_t) );
  void Free(void*, void (*)(void*) );
  void Feed(void*, int, std::string*, Parser*);
};

using namespace SymplexLemon;

void Parser::parse(char *begin, char *end)
{
  result = NULL;
  lemParser = Alloc(malloc);
  p = begin;
  pe = eof = end;
  scan();
  Feed(lemParser, 0, NULL, this);
  Free(lemParser, free);
}

void Parser::tok(int t)
{
  std::string *tok_string = new std::string(ts, te);
  //std::cout << "feeding token " << t << " " << *tok_string << "\n";
  Feed(lemParser, t, tok_string, this);
}
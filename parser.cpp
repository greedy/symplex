/*
 *  Copyright 2013 Geoffrey Reedy
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cstdlib>
#include <string>
#include <iostream>

#include "parser.hpp"
#include "ast.hpp"

namespace SymplexLemon {
  void *Alloc( void* (*)(size_t) );
  void Free(void*, void (*)(void*) );
  void Feed(void*, int, std::string*, Parser*);
};

using namespace SymplexLemon;

Parser::~Parser() {
  if (lemParser) { Free(lemParser, free); }
  delete result;
}

void Parser::parse(char *begin, char *end)
{
  result = NULL;
  if (lemParser) { Free(lemParser, free); lemParser = NULL; }
  lemParser = Alloc(malloc);
  p = begin;
  pe = eof = end;
  scan();
  Feed(lemParser, 0, NULL, this);
}

void Parser::tok(int t)
{
  std::string *tok_string = new std::string(ts, te);
  //std::cout << "feeding token " << t << " " << *tok_string << "\n";
  Feed(lemParser, t, tok_string, this);
}

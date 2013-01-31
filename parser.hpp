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
#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>

class Program;

struct TokenData {
  std::string *lexeme;
  unsigned line, column;
  TokenData(std::string *lexeme_, unsigned line_, unsigned column_)
    : lexeme(lexeme_), line(line_), column(column_) {}
};

class Parser {
  void scan();
  void tok(int t);

  int cs, act;
  char *p, *pe, *eof, *ts, *te;
  char *linestart;
  unsigned curline = 0;

  void *lemParser = 0;
public:
  ~Parser();
  Program *result;
  void parse(char *begin, char *end);
};

#endif

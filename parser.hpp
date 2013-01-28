#ifndef PARSER_HPP
#define PARSER_HPP

class Program;

class Parser {
  void scan();
  void tok(int t);

  int cs, act;
  char *p, *pe, *eof, *ts, *te;

  void *lemParser;
public:
  Program *result;
  void parse(char *begin, char *end);
};

#endif

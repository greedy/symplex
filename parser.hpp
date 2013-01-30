#ifndef PARSER_HPP
#define PARSER_HPP

class Program;

class Parser {
  void scan();
  void tok(int t);

  int cs, act;
  char *p, *pe, *eof, *ts, *te;

  void *lemParser = 0;
public:
  ~Parser();
  Program *result;
  void parse(char *begin, char *end);
};

#endif

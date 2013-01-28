#include "parser.hpp"

#include "grammar.h"

%%{
machine symplex;

main := |*
  'function' { tok(FUNCTION); };
  'suite' { tok(SUITE); };
  'test' { tok(TEST); };
  'if' { tok(IF); };
  'else' { tok(ELSE); };

  '#' { tok(SHARP); };

  '{' { tok(LBRACE); };
  '}' { tok(RBRACE); };
  '(' { tok(LPAREN); };
  ')' { tok(RPAREN); };
  ';' { tok(SEMI); };
  ',' { tok(COMMA); };

  '+' { tok(PLUS); };
  '-' { tok(MINUS); };
  '*' { tok(TIMES); };
  '/' { tok(DIVIDE); };
  '%' { tok(MOD); };

  '==' { tok(EQ); };
  '<' { tok(LT); };
  '>' { tok(GT); };
  '<=' { tok(LE); };
  '>=' { tok(GE); };
  '!=' { tok(NE); };

  '=' { tok(ASSIGN); };

  ('+'|'-')? digit+ { tok(INT); };

  (alpha | '_') (alnum | '_')* { tok(IDENT); };

  space;
*|;

}%%

%% write data;

void Parser::scan() {
  %% write init;
  while (p != eof) {
    %% write exec;
  }
}

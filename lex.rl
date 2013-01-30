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

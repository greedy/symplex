
#line 1 "lex.rl"
#include "parser.hpp"

#include "grammar.h"


#line 46 "lex.rl"



#line 13 "lex.cpp"
static const int symplex_start = 2;
static const int symplex_first_final = 2;
static const int symplex_error = 0;

static const int symplex_en_main = 2;


#line 49 "lex.rl"

void Parser::scan() {
  
#line 25 "lex.cpp"
	{
	cs = symplex_start;
	ts = 0;
	te = 0;
	act = 0;
	}

#line 52 "lex.rl"
  while (p != eof) {
    
#line 36 "lex.cpp"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
tr0:
#line 35 "lex.rl"
	{te = p+1;{ tok(NE); }}
	goto st2;
tr2:
#line 43 "lex.rl"
	{te = p+1;}
	goto st2;
tr4:
#line 15 "lex.rl"
	{te = p+1;{ tok(SHARP); }}
	goto st2;
tr5:
#line 28 "lex.rl"
	{te = p+1;{ tok(MOD); }}
	goto st2;
tr6:
#line 19 "lex.rl"
	{te = p+1;{ tok(LPAREN); }}
	goto st2;
tr7:
#line 20 "lex.rl"
	{te = p+1;{ tok(RPAREN); }}
	goto st2;
tr8:
#line 26 "lex.rl"
	{te = p+1;{ tok(TIMES); }}
	goto st2;
tr10:
#line 22 "lex.rl"
	{te = p+1;{ tok(COMMA); }}
	goto st2;
tr12:
#line 27 "lex.rl"
	{te = p+1;{ tok(DIVIDE); }}
	goto st2;
tr14:
#line 21 "lex.rl"
	{te = p+1;{ tok(SEMI); }}
	goto st2;
tr24:
#line 17 "lex.rl"
	{te = p+1;{ tok(LBRACE); }}
	goto st2;
tr25:
#line 18 "lex.rl"
	{te = p+1;{ tok(RBRACE); }}
	goto st2;
tr26:
#line 1 "NONE"
	{	switch( act ) {
	case 1:
	{{p = ((te))-1;} tok(FUNCTION); }
	break;
	case 2:
	{{p = ((te))-1;} tok(SUITE); }
	break;
	case 3:
	{{p = ((te))-1;} tok(TEST); }
	break;
	case 4:
	{{p = ((te))-1;} tok(IF); }
	break;
	case 5:
	{{p = ((te))-1;} tok(ELSE); }
	break;
	case 13:
	{{p = ((te))-1;} tok(PLUS); }
	break;
	case 14:
	{{p = ((te))-1;} tok(MINUS); }
	break;
	case 25:
	{{p = ((te))-1;} tok(INT); }
	break;
	case 26:
	{{p = ((te))-1;} tok(IDENT); }
	break;
	}
	}
	goto st2;
tr27:
#line 31 "lex.rl"
	{te = p;p--;{ tok(LT); }}
	goto st2;
tr28:
#line 33 "lex.rl"
	{te = p+1;{ tok(LE); }}
	goto st2;
tr29:
#line 37 "lex.rl"
	{te = p;p--;{ tok(ASSIGN); }}
	goto st2;
tr30:
#line 30 "lex.rl"
	{te = p+1;{ tok(EQ); }}
	goto st2;
tr31:
#line 32 "lex.rl"
	{te = p;p--;{ tok(GT); }}
	goto st2;
tr32:
#line 34 "lex.rl"
	{te = p+1;{ tok(GE); }}
	goto st2;
tr33:
#line 41 "lex.rl"
	{te = p;p--;{ tok(IDENT); }}
	goto st2;
st2:
#line 1 "NONE"
	{ts = 0;}
	if ( ++p == pe )
		goto _test_eof2;
case 2:
#line 1 "NONE"
	{ts = p;}
#line 159 "lex.cpp"
	switch( (*p) ) {
		case 32: goto tr2;
		case 33: goto st1;
		case 35: goto tr4;
		case 37: goto tr5;
		case 40: goto tr6;
		case 41: goto tr7;
		case 42: goto tr8;
		case 43: goto tr9;
		case 44: goto tr10;
		case 45: goto tr11;
		case 47: goto tr12;
		case 59: goto tr14;
		case 60: goto st4;
		case 61: goto st5;
		case 62: goto st6;
		case 95: goto tr18;
		case 101: goto st8;
		case 102: goto st11;
		case 105: goto st18;
		case 115: goto st19;
		case 116: goto st23;
		case 123: goto tr24;
		case 125: goto tr25;
	}
	if ( (*p) < 48 ) {
		if ( 9 <= (*p) && (*p) <= 13 )
			goto tr2;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto tr18;
		} else if ( (*p) >= 65 )
			goto tr18;
	} else
		goto tr13;
	goto st0;
st0:
cs = 0;
	goto _out;
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
	if ( (*p) == 61 )
		goto tr0;
	goto st0;
tr9:
#line 1 "NONE"
	{te = p+1;}
#line 24 "lex.rl"
	{act = 13;}
	goto st3;
tr11:
#line 1 "NONE"
	{te = p+1;}
#line 25 "lex.rl"
	{act = 14;}
	goto st3;
tr13:
#line 1 "NONE"
	{te = p+1;}
#line 39 "lex.rl"
	{act = 25;}
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 229 "lex.cpp"
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr13;
	goto tr26;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
	if ( (*p) == 61 )
		goto tr28;
	goto tr27;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
	if ( (*p) == 61 )
		goto tr30;
	goto tr29;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
	if ( (*p) == 61 )
		goto tr32;
	goto tr31;
tr18:
#line 1 "NONE"
	{te = p+1;}
#line 41 "lex.rl"
	{act = 26;}
	goto st7;
tr36:
#line 1 "NONE"
	{te = p+1;}
#line 13 "lex.rl"
	{act = 5;}
	goto st7;
tr43:
#line 1 "NONE"
	{te = p+1;}
#line 9 "lex.rl"
	{act = 1;}
	goto st7;
tr44:
#line 1 "NONE"
	{te = p+1;}
#line 12 "lex.rl"
	{act = 4;}
	goto st7;
tr48:
#line 1 "NONE"
	{te = p+1;}
#line 10 "lex.rl"
	{act = 2;}
	goto st7;
tr51:
#line 1 "NONE"
	{te = p+1;}
#line 11 "lex.rl"
	{act = 3;}
	goto st7;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
#line 294 "lex.cpp"
	if ( (*p) == 95 )
		goto tr18;
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr18;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr18;
	} else
		goto tr18;
	goto tr26;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
	switch( (*p) ) {
		case 95: goto tr18;
		case 108: goto st9;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr18;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr18;
	} else
		goto tr18;
	goto tr33;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
	switch( (*p) ) {
		case 95: goto tr18;
		case 115: goto st10;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr18;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr18;
	} else
		goto tr18;
	goto tr33;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
	switch( (*p) ) {
		case 95: goto tr18;
		case 101: goto tr36;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr18;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr18;
	} else
		goto tr18;
	goto tr33;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
	switch( (*p) ) {
		case 95: goto tr18;
		case 117: goto st12;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr18;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr18;
	} else
		goto tr18;
	goto tr33;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
	switch( (*p) ) {
		case 95: goto tr18;
		case 110: goto st13;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr18;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr18;
	} else
		goto tr18;
	goto tr33;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
	switch( (*p) ) {
		case 95: goto tr18;
		case 99: goto st14;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr18;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr18;
	} else
		goto tr18;
	goto tr33;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
	switch( (*p) ) {
		case 95: goto tr18;
		case 116: goto st15;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr18;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr18;
	} else
		goto tr18;
	goto tr33;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
	switch( (*p) ) {
		case 95: goto tr18;
		case 105: goto st16;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr18;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr18;
	} else
		goto tr18;
	goto tr33;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
	switch( (*p) ) {
		case 95: goto tr18;
		case 111: goto st17;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr18;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr18;
	} else
		goto tr18;
	goto tr33;
st17:
	if ( ++p == pe )
		goto _test_eof17;
case 17:
	switch( (*p) ) {
		case 95: goto tr18;
		case 110: goto tr43;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr18;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr18;
	} else
		goto tr18;
	goto tr33;
st18:
	if ( ++p == pe )
		goto _test_eof18;
case 18:
	switch( (*p) ) {
		case 95: goto tr18;
		case 102: goto tr44;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr18;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr18;
	} else
		goto tr18;
	goto tr33;
st19:
	if ( ++p == pe )
		goto _test_eof19;
case 19:
	switch( (*p) ) {
		case 95: goto tr18;
		case 117: goto st20;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr18;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr18;
	} else
		goto tr18;
	goto tr33;
st20:
	if ( ++p == pe )
		goto _test_eof20;
case 20:
	switch( (*p) ) {
		case 95: goto tr18;
		case 105: goto st21;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr18;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr18;
	} else
		goto tr18;
	goto tr33;
st21:
	if ( ++p == pe )
		goto _test_eof21;
case 21:
	switch( (*p) ) {
		case 95: goto tr18;
		case 116: goto st22;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr18;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr18;
	} else
		goto tr18;
	goto tr33;
st22:
	if ( ++p == pe )
		goto _test_eof22;
case 22:
	switch( (*p) ) {
		case 95: goto tr18;
		case 101: goto tr48;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr18;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr18;
	} else
		goto tr18;
	goto tr33;
st23:
	if ( ++p == pe )
		goto _test_eof23;
case 23:
	switch( (*p) ) {
		case 95: goto tr18;
		case 101: goto st24;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr18;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr18;
	} else
		goto tr18;
	goto tr33;
st24:
	if ( ++p == pe )
		goto _test_eof24;
case 24:
	switch( (*p) ) {
		case 95: goto tr18;
		case 115: goto st25;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr18;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr18;
	} else
		goto tr18;
	goto tr33;
st25:
	if ( ++p == pe )
		goto _test_eof25;
case 25:
	switch( (*p) ) {
		case 95: goto tr18;
		case 116: goto tr51;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr18;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr18;
	} else
		goto tr18;
	goto tr33;
	}
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof1: cs = 1; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 
	_test_eof9: cs = 9; goto _test_eof; 
	_test_eof10: cs = 10; goto _test_eof; 
	_test_eof11: cs = 11; goto _test_eof; 
	_test_eof12: cs = 12; goto _test_eof; 
	_test_eof13: cs = 13; goto _test_eof; 
	_test_eof14: cs = 14; goto _test_eof; 
	_test_eof15: cs = 15; goto _test_eof; 
	_test_eof16: cs = 16; goto _test_eof; 
	_test_eof17: cs = 17; goto _test_eof; 
	_test_eof18: cs = 18; goto _test_eof; 
	_test_eof19: cs = 19; goto _test_eof; 
	_test_eof20: cs = 20; goto _test_eof; 
	_test_eof21: cs = 21; goto _test_eof; 
	_test_eof22: cs = 22; goto _test_eof; 
	_test_eof23: cs = 23; goto _test_eof; 
	_test_eof24: cs = 24; goto _test_eof; 
	_test_eof25: cs = 25; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch ( cs ) {
	case 3: goto tr26;
	case 4: goto tr27;
	case 5: goto tr29;
	case 6: goto tr31;
	case 7: goto tr26;
	case 8: goto tr33;
	case 9: goto tr33;
	case 10: goto tr33;
	case 11: goto tr33;
	case 12: goto tr33;
	case 13: goto tr33;
	case 14: goto tr33;
	case 15: goto tr33;
	case 16: goto tr33;
	case 17: goto tr33;
	case 18: goto tr33;
	case 19: goto tr33;
	case 20: goto tr33;
	case 21: goto tr33;
	case 22: goto tr33;
	case 23: goto tr33;
	case 24: goto tr33;
	case 25: goto tr33;
	}
	}

	_out: {}
	}

#line 54 "lex.rl"
  }
}

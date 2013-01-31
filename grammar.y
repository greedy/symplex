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
%include {#include <cassert>}
%include {#include <string>}
%include {#include <memory>}
%include {#include <algorithm>}
%include {#include "ast.hpp"}
%include {#include "parser.hpp"}
%include {#include <iostream>}

%right ASSIGN.
%left SHARP.
%left EQ NE GT GE LT LE.
%left PLUS MINUS.
%left TIMES DIVIDE MOD.

%name SymplexLemon

%extra_argument {Parser* p}

%token_type { TokenData* }
%token_destructor { delete $$; }

%syntax_error { std::cerr << "Syntax error!\n"; }
%parse_accept { std::cerr << "Parse successful!\n"; }
%parse_failure { std::cerr << "Parse failure!\n"; }

%type file {Program*}
%destructor file {delete $$;}
file ::= prog(P). { p->result = P; }

%type prog {Program*}
%destructor prog {delete $$;}
prog(P) ::= prog(O) suite(S). { P = O; O->suites.push_back(S); }
prog(P) ::= prog(O) fundef(F). { P = O; O->functions.push_back(F); }
prog(P) ::= . { P = new Program(); }

%type suite {Suite*}
%destructor suite {delete $$;}
suite(S) ::= SUITE IDENT(N) LBRACE exprlist(I) tests(T) RBRACE. { S = new Suite(N->lexeme, I, T); }

%type exprlist {std::vector<Expr*>*}
%destructor exprlist { destroy($$); }
exprlist(L) ::= exprlist(O) expr(E) SEMI. { L = O; L->push_back(E); }
exprlist(L) ::= . { L = new std::vector<Expr*>; }

%type tests {std::vector<Test*>*}
%destructor tests { destroy($$); }
tests(C) ::= tests(O) test(T). { C = O; O->push_back(T); }
tests(C) ::= test(T). { C = new std::vector<Test*> { T }; }

%type test {Test*}
%destructor test { delete $$; }
test(T) ::= TEST IDENT(N) block(B). { T = new Test(N->lexeme, B); }

%type fundef {Function*}
%destructor fundef {delete $$;}
fundef(F) ::= FUNCTION IDENT(N) LPAREN params(P) RPAREN block(B). { F = new Function(N->lexeme, P, B); }
fundef(F) ::= FUNCTION IDENT(N) LPAREN RPAREN block(B). { F = new Function(N->lexeme, B); }

%type params {std::vector<std::string*>*}
%destructor params { destroy($$); }
params(L) ::= IDENT(I). { L = new std::vector<std::string*> { I->lexeme }; }
params(L) ::= params(O) COMMA IDENT(I). { L = O; L->push_back(I->lexeme); }

%type args {std::vector<Expr*>*}
%destructor args { destroy($$); }
args(A) ::= expr(E). { A = new std::vector<Expr*> { E }; }
args(A) ::= args(O) COMMA expr(E). { A = O; A->push_back(E); }

%type expr {Expr*}
%destructor expr {delete $$;}
expr(R) ::= LPAREN expr(E) RPAREN. { R = E; }
expr(R) ::= IDENT(N) ASSIGN expr(E). { R = new AssignExpr(N->lexeme, E); }
expr(R) ::= expr(A) PLUS expr(B). { R = new BasicExpr(BasicExpr::Plus, {A,B}); }
expr(R) ::= expr(A) MINUS expr(B). { R = new BasicExpr(BasicExpr::Minus, {A,B}); }
expr(R) ::= expr(A) TIMES expr(B). { R = new BasicExpr(BasicExpr::Times, {A,B}); }
expr(R) ::= expr(A) DIVIDE expr(B). { R = new BasicExpr(BasicExpr::Divide, {A,B}); }
expr(R) ::= expr(A) MOD expr(B). { R = new BasicExpr(BasicExpr::Mod, {A,B}); }
expr(R) ::= expr(A) EQ expr(B). { R = new BasicExpr(BasicExpr::Eq, {A,B}); }
expr(R) ::= expr(A) LT expr(B). { R = new BasicExpr(BasicExpr::Lt, {A,B}); }
expr(R) ::= expr(A) GT expr(B). { R = new BasicExpr(BasicExpr::Gt, {A,B}); }
expr(R) ::= expr(A) LE expr(B). { R = new BasicExpr(BasicExpr::Le, {A,B}); }
expr(R) ::= expr(A) GE expr(B). { R = new BasicExpr(BasicExpr::Ge, {A,B}); }
expr(R) ::= expr(A) NE expr(B). { R = new BasicExpr(BasicExpr::Ne, {A,B}); }
expr(R) ::= IDENT(I) LPAREN RPAREN. { R = new CallExpr(I->lexeme); }
expr(R) ::= IDENT(I) LPAREN args(A) RPAREN. { R = new CallExpr(I->lexeme, A); }
expr(R) ::= IF LPAREN expr(C) RPAREN block(T) ELSE block(F). { R = new IfThenElseExpr(C, T, F); }
expr(R) ::= INT(I). { R = new ConstExpr(I->lexeme); }
expr(R) ::= IDENT(I). { R = new VarExpr(I->lexeme); }

%type block {std::vector<Expr*>*}
%destructor block {destroy($$);}
block(B) ::= LBRACE exprlist(L) RBRACE. {B=L;}

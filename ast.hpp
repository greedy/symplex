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
#ifndef AST_HPP
#define AST_HPP

#include <vector>
#include <string>
#include <algorithm>
#include <memory>

template <typename T>
void destroy(std::vector<T*> *c)
{
  std::for_each(c->begin(), c->end(), std::default_delete<T>());
  delete c;
}

struct Expr {
  virtual ~Expr() {};
};
struct AssignExpr : Expr {
  AssignExpr(std::string *name_, Expr *expr_) : name(name_), expr(expr_) {};
  ~AssignExpr() {
    delete(name);
    delete(expr);
  };

  std::string *name;
  Expr *expr;
};
struct IfThenElseExpr : Expr {
  IfThenElseExpr(Expr *cond_, std::vector<Expr*> *iftrue_, std::vector<Expr*> *iffalse_)
    : cond(cond_), iftrue(iftrue_), iffalse(iffalse_) {};
  ~IfThenElseExpr() {
    delete cond;
    destroy(iftrue);
    destroy(iffalse);
  };

  Expr *cond;
  std::vector<Expr*> *iftrue;
  std::vector<Expr*> *iffalse;
};
struct CallExpr : Expr {
  CallExpr(std::string *target_, std::vector<Expr*>* args_)
    : target(target_), args(args_) {};
  CallExpr(std::string *target_)
    : CallExpr(target_, new std::vector<Expr*>()) {};
  ~CallExpr() {
    delete target;
    destroy(args);
  }

  std::string *target;
  std::vector<Expr*> *args;
};
struct BasicExpr : Expr {
  enum Operation {
    Plus,
    Minus,
    Times,
    Divide,
    Mod,
    Eq,
    Lt,
    Gt,
    Le,
    Ge,
    Ne
  };

  BasicExpr(Operation op_, std::initializer_list<Expr*> children_)
    : op(op_), children(new std::vector<Expr*>(children_)) {};
  ~BasicExpr() {
    destroy(children);
  }

  Operation op;
  std::vector<Expr*> *children;
};
struct ConstExpr : Expr {
  ConstExpr(std::string *cv_) : cv(cv_) {};
  ~ConstExpr() {
    delete cv;
  }
  
  std::string *cv;
};
struct VarExpr : Expr {
  VarExpr(std::string *var_) : var(var_) {};
  ~VarExpr() {
    delete var;
  }
  std::string *var;
};
struct LabeledExpr : Expr {
  LabeledExpr(std::string *label_, Expr *expr_) : label(label_), expr(expr_) {}
  ~LabeledExpr() {
    delete label;
    delete expr;
  }
  std::string *label;
  Expr *expr;
};
struct Function {
  Function(std::string *name_, std::vector<std::string*> *params_, std::vector<Expr*> *body_)
    : name(name_), params(params_), body(body_) {};
  Function(std::string *name_, std::vector<Expr*> *body_)
    : Function(name_, new std::vector<std::string*>(), body_) {};
  ~Function() {
    delete name;
    destroy(params);
    destroy(body);
  }

  std::string *name;
  std::vector<std::string*> *params;
  std::vector<Expr*> *body;
};
struct Test {
  Test(std::string *name_, std::vector<Expr*> *body_) : name(name_), body(body_) {};
  ~Test() {
    delete name;
    destroy(body);
  }

  std::string *name;
  std::vector<Expr*> *body;
};
struct Suite {
  Suite(std::string *name_, std::vector<Expr*> *setup_, std::vector<Test*> *tests_)
    : name(name_), setup(setup_), tests(tests_) {};
  ~Suite() {
    delete name;
    destroy(setup);
    destroy(tests);
  }

  std::string *name;
  std::vector<Expr*> *setup;
  std::vector<Test*> *tests;
};
struct Program {
  ~Program() {
    std::for_each(functions.begin(), functions.end(), std::default_delete<Function>());
    std::for_each(suites.begin(), suites.end(), std::default_delete<Suite>());
  }
  std::vector<Function*> functions;
  std::vector<Suite*> suites;
};

#endif

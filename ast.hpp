#ifndef AST_HPP
#define AST_HPP

#include <vector>
#include <string>

struct Expr {
  virtual ~Expr();
};
struct AssignExpr : Expr {
  AssignExpr(std::string *name_, Expr *expr_) : name(name_), expr(expr_) {};

  std::string *name;
  Expr *expr;
};
struct IfThenElseExpr : Expr {
  IfThenElseExpr(Expr *cond_, std::vector<Expr*> *iftrue_, std::vector<Expr*> *iffalse_)
    : cond(cond_), iftrue(iftrue_), iffalse(iffalse_) {};

  Expr *cond;
  std::vector<Expr*> *iftrue;
  std::vector<Expr*> *iffalse;
};
struct CallExpr : Expr {
  CallExpr(std::string *target_, std::vector<Expr*>* args_)
    : target(target_), args(args_) {};
  CallExpr(std::string *target_)
    : CallExpr(target_, new std::vector<Expr*>()) {};

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

  Operation op;
  std::vector<Expr*> *children;
};
struct ConstExpr : Expr {
  ConstExpr(std::string *cv_) : cv(cv_) {};
  std::string *cv;
};
struct VarExpr : Expr {
  VarExpr(std::string *var_) : var(var_) {};
  std::string *var;
};
struct LabeledExpr : Expr {
  LabeledExpr(std::string *label_, Expr *expr_) : label(label_), expr(expr_) {}
  std::string *label;
  Expr *expr;
};
struct Function {
  Function(std::string *name_, std::vector<std::string*> *params_, std::vector<Expr*> *body_)
    : name(name_), params(params_), body(body_) {};
  Function(std::string *name_, std::vector<Expr*> *body_)
    : Function(name_, new std::vector<std::string*>(), body_) {};

  std::string *name;
  std::vector<std::string*> *params;
  std::vector<Expr*> *body;
};
struct Test {
  Test(std::vector<Expr*> *body_) : body(body_) {};

  std::vector<Expr*> *body;
};
struct Suite {
  Suite(std::vector<Expr*> *setup_, std::vector<Test*> *tests_)
    : setup(setup_), tests(tests_) {};

  std::vector<Expr*> *setup;
  std::vector<Test*> *tests;
};
struct Program {
  std::vector<Function*> functions;
  std::vector<Suite*> suites;
};

#endif

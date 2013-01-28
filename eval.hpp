#ifndef EVAL_HPP
#define EVAL_HPP

#include "ast.hpp"

#include <cvc3/vc.h>

#include <unordered_set>
#include <unordered_map>
#include <iostream>

class State;
class Runner;

struct Activation {
  std::unordered_map<std::string, CVC3::Expr> env;
  State *owner;
  std::string name;

  Activation(State *owner_, std::string name_) : owner(owner_), name(name_) {}
};

struct State {
  Runner *owner;
  std::vector<Activation> callstack;
  CVC3::Expr evaluate(std::vector<Expr*> *e) { return evaluate(*e); }
  CVC3::Expr evaluate(std::vector<Expr*> &e);
  CVC3::Expr evaluate(Expr *e);
  CVC3::Expr fun_symbolic();
  CVC3::Expr fun_assume(CVC3::Expr e);
  CVC3::Expr fun_assert(CVC3::Expr e);
  Activation &curframe() { return callstack.back(); }

  State(Runner *owner_)
    : owner(owner_)
  {
    callstack.emplace_back(this, "entry");
  }
};

enum Status {
  MUST_BE_TRUE,
  MUST_BE_FALSE,
  TRUE_OR_FALSE,
  NOT_TRUE_NOR_FALSE,
  UNKNOWN
};

struct Runner {
  std::unordered_map<std::string, Function*> func_byname;
  CVC3::ValidityChecker *solver;
  void run(Program *p);
  Status checkStatus(CVC3::Expr e);

  Runner() : solver(CVC3::ValidityChecker::create()) {};
};

#endif

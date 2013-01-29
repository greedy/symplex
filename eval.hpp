#ifndef EVAL_HPP
#define EVAL_HPP

#include "ast.hpp"

#include <cvc3/vc.h>

#include <vector>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include <iostream>

class State;
class Runner;

struct Instruction;

struct MachineState {
  Runner *runner;
  typedef std::unordered_map<std::string, CVC3::Expr> environment;
  std::vector<CVC3::Expr> opstack;
  std::vector<environment> envstack;
  environment &env() { return envstack.back(); }
  std::list<Instruction*> code;
  std::vector<CVC3::Expr> assumptions;
};

struct CompiledFunction {
  std::vector<std::string> params;
  std::vector<Instruction*> code;
};

struct Instruction {
  virtual ~Instruction() {};
  virtual void execute(MachineState &s) = 0;
};

namespace Instructions {
  struct Store : Instruction {
    std::string dest;
    void execute(MachineState &s);
  };
  struct IfThenElse : Instruction {
    std::vector<Instruction*> iftrue, iffalse;
    void execute(MachineState &s);
  };
  struct Call : Instruction {
    std::string target;
    void execute(MachineState &s);
  };
  struct Return : Instruction {
    void execute(MachineState &s);
  };
  struct Add : Instruction {};
  struct Sub : Instruction {};
  struct Mult : Instruction {};
  struct Div : Instruction {};
  struct Mod : Instruction {};
  struct Eq : Instruction {};
  struct Lt : Instruction {};
  struct Gt : Instruction {};
  struct Le : Instruction {};
  struct Ge : Instruction {};
  struct Ne : Instruction {};
  struct LoadConst : Instruction {
    std::string value;
  };
  struct LoadVar : Instruction {
    std::string name;
  };
  struct Pop : Instruction {};
  struct LoadSymbolic : Instruction {};
  struct Check : Instruction {};
  struct Assume : Instruction {};
}

void compile(std::vector<Expr*> &exprs, std::vector<Instruction*> &instructions);

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
  CVC3::Expr fun_check(CVC3::Expr e);
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
  std::unordered_map<std::string, CompiledFunction*> compfunc_byname;
  CVC3::ValidityChecker *solver;
  void run(Program *p);
  Status checkStatus(CVC3::Expr e);

  MachineState& fork(MachineState &s);

  Runner() : solver(CVC3::ValidityChecker::create()) {};
};

#endif

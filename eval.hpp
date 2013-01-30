#ifndef EVAL_HPP
#define EVAL_HPP

#include "ast.hpp"

#include <cvc3/vc.h>

#include <list>
#include <vector>
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
  std::vector<std::string> trace;
  environment &env() { return envstack.back(); }
  std::list<Instruction*> code;
  std::vector<CVC3::Expr> assumptions;
};

struct CompiledFunction {
  std::string name;
  std::vector<std::string> params;
  std::vector<Instruction*> code;
};

struct CompiledTest {
  std::string name;
  std::vector<Instruction*> code;
};

struct CompiledSuite {
  std::string name;
  std::vector<Instruction*> setup_code;
  std::vector<CompiledTest> tests;
};

struct Instruction {
  virtual ~Instruction() {};
  virtual void execute(MachineState &s) = 0;
};

namespace Instructions {
  struct Store : Instruction {
    std::string dest;
    void execute(MachineState &s);
    Store(std::string &dest_) : dest(dest_) {}
  };
  struct IfThenElse : Instruction {
    std::vector<Instruction*> iftrue, iffalse;
    void execute(MachineState &s);
    IfThenElse(std::vector<Instruction*> &iftrue_, std::vector<Instruction*> &iffalse_)
      : iftrue(iftrue_), iffalse(iffalse_) {}
  };
  struct Call : Instruction {
    std::string target;
    void execute(MachineState &s);
    Call(std::string &target_) : target(target_) {}
  };
  struct Return : Instruction {
    void execute(MachineState &s);
  };
  struct Add : Instruction {
    void execute(MachineState &s);
  };
  struct Sub : Instruction {
    void execute(MachineState &s);
  };
  struct Mult : Instruction {
    void execute(MachineState &s);
  };
  struct Div : Instruction {
    void execute(MachineState &s);
  };
  struct Mod : Instruction {
    void execute(MachineState &s);
  };
  struct Eq : Instruction {
    void execute(MachineState &s);
  };
  struct Lt : Instruction {
    void execute(MachineState &s);
  };
  struct Gt : Instruction {
    void execute(MachineState &s);
  };
  struct Le : Instruction {
    void execute(MachineState &s);
  };
  struct Ge : Instruction {
    void execute(MachineState &s);
  };
  struct Ne : Instruction {
    void execute(MachineState &s);
  };
  struct LoadConst : Instruction {
    long value;
    void execute(MachineState &s);
    LoadConst(long value_) : value(value_) {}
  };
  struct LoadVar : Instruction {
    std::string name;
    void execute(MachineState &s);
    LoadVar(std::string &name_) : name(name_) {}
  };
  struct Pop : Instruction {
    void execute(MachineState &s);
  };
  struct LoadSymbolic : Instruction {
    void execute(MachineState &s);
  };
  struct Check : Instruction {
    void execute(MachineState &s);
  };
  struct Assume : Instruction {
    void execute(MachineState &s);
  };
}

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
  std::unordered_map<std::string, CompiledFunction*> compfunc_byname;
  std::vector<CompiledSuite> suites;
  CVC3::ValidityChecker *solver;
  Status checkStatus(CVC3::Expr e);

  std::list<MachineState> active_states;

  MachineState& fork(MachineState &s);

  Runner(Program *p) : solver(CVC3::ValidityChecker::create()) {
    compile(p);
  };

  void runSuites();

private:
  void compile(Program *p);
  void compileExpr(Expr *e, std::vector<Instruction*> &code);
  void compileBody(std::vector<Expr*> &body, std::vector<Instruction*> &code);
  void runStates();
};

#endif

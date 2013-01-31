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
#ifndef EVAL_HPP
#define EVAL_HPP

#include "ast.hpp"

extern "C" {
#include <boolector.h>
}

#include <list>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <memory>

class State;
class Runner;

struct Instruction;

enum Status {
  MUST_BE_TRUE,
  MUST_BE_FALSE,
  TRUE_OR_FALSE,
  NOT_TRUE_NOR_FALSE,
  UNKNOWN
};

struct MachineState {
  Runner *runner;
  Status checkStatus(BtorNode *e);
  typedef std::unordered_map<std::string, BtorNode*> environment;
  std::vector<BtorNode*> opstack;
  std::vector<environment> envstack;
  std::vector<std::string> trace;
  environment &env() { return envstack.back(); }
  std::list<Instruction*> code;
  std::vector<BtorNode*> assumptions;
  void apply_assumptions();
  void release(BtorNode *n);
  BtorNode *copy(BtorNode *n);

  ~MachineState() {
    for (auto &e : envstack) {
      for (auto &p : e) {
	release(p.second);
      }
    }
    for (auto &a : assumptions) {
      release(a);
    }
    for (auto &o : opstack) {
      release(o);
    }
  }

  MachineState() = default;

  MachineState(const MachineState &other)
    : runner(other.runner), envstack(other.envstack), trace(other.trace)
    , code(other.code), assumptions(other.assumptions), opstack(other.opstack)
  {
    for (auto &e : envstack) {
      for (auto &p : e) {
	p.second = copy(p.second);
      }
    }
    for (auto &a : assumptions) {
      a = copy(a);
    }
    for (auto &o : opstack) {
      o = copy(o);
    }
  }
};

struct CompiledFunction {
  std::string name;
  std::vector<std::string> params;
  std::vector<std::unique_ptr<Instruction> > code;
};

struct CompiledTest {
  std::string name;
  std::vector<std::unique_ptr<Instruction> > code;
};

struct CompiledSuite {
  std::string name;
  std::vector<std::unique_ptr<Instruction> > setup_code;
  std::vector<CompiledTest> tests;
};

struct Instruction {
  virtual ~Instruction() {};
  virtual void execute(MachineState &s) = 0;
  virtual std::vector<std::string> source() = 0;
};

namespace Instructions {
  struct Store : Instruction {
    std::string dest;
    void execute(MachineState &s);
    Store(std::string &dest_) : dest(dest_) {}
    std::vector<std::string> source() { return std::vector<std::string>({std::string("store ") + dest}); }
  };
  struct IfThenElse : Instruction {
    std::vector<std::unique_ptr<Instruction> > iftrue, iffalse;
    void execute(MachineState &s);
    std::vector<std::string> source() {
      std::vector<std::string> res;
      res.emplace_back("ifthenelse");
      res.emplace_back(" iftrue:");
      for (auto &i : iftrue) {
	for (auto &l : i->source()) {
	  res.emplace_back("  " + l);
	}
      }
      res.emplace_back(" iffalse:");
      for (auto &i : iffalse) {
	for (auto &l : i->source()) {
	  res.emplace_back("  " + l);
	}
      }
      return res;
    }
  };
  struct Call : Instruction {
    std::string target;
    void execute(MachineState &s);
    Call(std::string &target_) : target(target_) {}
    std::vector<std::string> source() {
      return std::vector<std::string>({"call " + target});
    }
  };
  struct Return : Instruction {
    void execute(MachineState &s);
    std::vector<std::string> source() {
      return std::vector<std::string>({"return"});
    }
  };
  struct Add : Instruction {
    void execute(MachineState &s);
    std::vector<std::string> source() {
      return std::vector<std::string>({"add"});
    }
  };
  struct Sub : Instruction {
    void execute(MachineState &s);
    std::vector<std::string> source() {
      return std::vector<std::string>({"sub"});
    }
  };
  struct Mult : Instruction {
    void execute(MachineState &s);
    std::vector<std::string> source() {
      return std::vector<std::string>({"mult"});
    }
  };
  struct Div : Instruction {
    void execute(MachineState &s);
    std::vector<std::string> source() {
      return std::vector<std::string>({"div"});
    }
  };
  struct Mod : Instruction {
    void execute(MachineState &s);
    std::vector<std::string> source() {
      return std::vector<std::string>({"mod"});
    }
  };
  struct Eq : Instruction {
    void execute(MachineState &s);
    std::vector<std::string> source() {
      return std::vector<std::string>({"eq"});
    }
  };
  struct Lt : Instruction {
    void execute(MachineState &s);
    std::vector<std::string> source() {
      return std::vector<std::string>({"lt"});
    }
  };
  struct Gt : Instruction {
    void execute(MachineState &s);
    std::vector<std::string> source() {
      return std::vector<std::string>({"gt"});
    }
  };
  struct Le : Instruction {
    void execute(MachineState &s);
    std::vector<std::string> source() {
      return std::vector<std::string>({"le"});
    }
  };
  struct Ge : Instruction {
    void execute(MachineState &s);
    std::vector<std::string> source() {
      return std::vector<std::string>({"ge"});
    }
  };
  struct Ne : Instruction {
    void execute(MachineState &s);
    std::vector<std::string> source() {
      return std::vector<std::string>({"ne"});
    }
  };
  struct LoadConst : Instruction {
    long value;
    void execute(MachineState &s);
    LoadConst(long value_) : value(value_) {}
    std::vector<std::string> source() {
      return std::vector<std::string>({"load_const " + std::to_string(value)});
    }
  };
  struct LoadVar : Instruction {
    std::string name;
    void execute(MachineState &s);
    LoadVar(std::string &name_) : name(name_) {}
    std::vector<std::string> source() {
      return std::vector<std::string>({"load_var " + name});
    }
  };
  struct Pop : Instruction {
    void execute(MachineState &s);
    std::vector<std::string> source() {
      return std::vector<std::string>({"pop"});
    }
  };
  struct LoadSymbolic : Instruction {
    void execute(MachineState &s);
    std::vector<std::string> source() {
      return std::vector<std::string>({"load_symbolic"});
    }
  };
  struct Check : Instruction {
    void execute(MachineState &s);
    std::vector<std::string> source() {
      return std::vector<std::string>({"check"});
    }
  };
  struct Assume : Instruction {
    void execute(MachineState &s);
    std::vector<std::string> source() {
      return std::vector<std::string>({"assume"});
    }
  };
}

struct Runner {
  std::unordered_map<std::string, std::unique_ptr<CompiledFunction> > compfunc_byname;
  std::vector<CompiledSuite> suites;
  Btor *btor;

  std::list<MachineState> active_states;

  MachineState& fork(MachineState &s);

  Runner(Program *p) {
    btor = boolector_new();
    boolector_enable_model_gen(btor);
    boolector_enable_inc_usage(btor);
    compile(p);
  };

  ~Runner() {
    boolector_delete(btor);
  }

  void runSuites();

private:
  void compile(Program *p);
  void compileExpr(Expr *e, std::vector<std::unique_ptr<Instruction> > &code);
  void compileBody(std::vector<Expr*> &body, std::vector<std::unique_ptr<Instruction> > &code);
  void runStates();
};

inline void MachineState::release(BtorNode *n) { boolector_release(runner->btor, n); }
inline BtorNode *MachineState::copy(BtorNode *n) { return boolector_copy(runner->btor, n); }

#endif

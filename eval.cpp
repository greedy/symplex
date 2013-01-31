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
#include "eval.hpp"

#include <stdexcept>
#include <algorithm>
#include <iterator>

#define BV_WIDTH 8

struct assumptions_failed {};

template <typename C>
typename C::value_type pop(C &c) {
  typename C::value_type result = c.back();
  c.pop_back();
  return result;
}

template <typename C>
void push(C &c, typename C::value_type v) {
  c.push_back(v);
}

void splice(std::list<Instruction*> &to, std::vector<std::unique_ptr<Instruction> > &from) {
  for (auto it = from.rbegin(), ie = from.rend();
       it != ie; ++it) {
    to.push_front(it->get());
  }
}

namespace Instructions {
  void Store::execute(MachineState &s) {
    auto cell = s.env().find(dest);
    if (cell == s.env().end()) {
      s.env().insert(std::make_pair(dest, s.copy(s.opstack.back())));
    } else {
      s.release(cell->second);
      cell->second = s.copy(s.opstack.back());
    }
  }
  void IfThenElse::execute(MachineState &s) {
    BtorNode* cond = pop(s.opstack);
    Status stat = s.checkStatus(cond);
    switch (stat) {
    case MUST_BE_FALSE:
      splice(s.code, iffalse);
      break;
    case MUST_BE_TRUE:
      splice(s.code, iftrue);
      break;
    case TRUE_OR_FALSE:
      {
	MachineState &false_branch = s.runner->fork(s);
	false_branch.assumptions.push_back(boolector_not(s.runner->btor, cond));
	splice(false_branch.code, iffalse);
	s.assumptions.push_back(s.copy(cond));
	splice(s.code, iftrue);
      }
      break;
    case NOT_TRUE_NOR_FALSE:
      throw std::runtime_error("inconsistent state!");
      break;
    case UNKNOWN:
      throw std::runtime_error("branch condition has unknown status!");
      break;
    }
    s.release(cond);
  }
  void Call::execute(MachineState &s) {
    CompiledFunction &target_fun = *s.runner->compfunc_byname.at(target);
    s.envstack.emplace_back();
    for (auto it = target_fun.params.rbegin(), ie = target_fun.params.rend();
	 it != ie; ++it) {
      s.env().insert(std::make_pair(*it, pop(s.opstack)));
    }
    s.trace.push_back(target_fun.name);
    splice(s.code, target_fun.code);
  }
  void Return::execute(MachineState &s) {
    std::for_each(s.env().begin(), s.env().end(),
		  [&s](const std::pair<std::string,BtorNode*> &i){s.release(i.second);});
    s.envstack.pop_back();
    s.trace.pop_back();
  }
  void Add::execute(MachineState &s) {
    BtorNode* y = pop(s.opstack);
    BtorNode* x = pop(s.opstack);
    push(s.opstack, boolector_add(s.runner->btor, x, y));
    s.release(x);
    s.release(y);
  }
  void Sub::execute(MachineState &s) {
    BtorNode* y = pop(s.opstack);
    BtorNode* x = pop(s.opstack);
    push(s.opstack, boolector_sub(s.runner->btor, x, y));
    s.release(x);
    s.release(y);
  }
  void Mult::execute(MachineState &s) {
    BtorNode* y = pop(s.opstack);
    BtorNode* x = pop(s.opstack);
    push(s.opstack, boolector_mul(s.runner->btor, x, y));
    s.release(x);
    s.release(y);
  }
  void Div::execute(MachineState &s) {
    BtorNode* y = pop(s.opstack);
    BtorNode* x = pop(s.opstack);
    push(s.opstack, boolector_sdiv(s.runner->btor, x, y));
    s.release(x);
    s.release(y);
  }
  void Mod::execute(MachineState &s) {
    BtorNode* y = pop(s.opstack);
    BtorNode* x = pop(s.opstack);
    push(s.opstack, boolector_smod(s.runner->btor, x, y));
    s.release(x);
    s.release(y);
  }
  void Eq::execute(MachineState &s) {
    BtorNode* y = pop(s.opstack);
    BtorNode* x = pop(s.opstack);
    push(s.opstack, boolector_eq(s.runner->btor, x, y));
    s.release(x);
    s.release(y);
  }
  void Lt::execute(MachineState &s) {
    BtorNode* y = pop(s.opstack);
    BtorNode* x = pop(s.opstack);
    push(s.opstack, boolector_slt(s.runner->btor, x, y));
    s.release(x);
    s.release(y);
  }
  void Gt::execute(MachineState &s) {
    BtorNode* y = pop(s.opstack);
    BtorNode* x = pop(s.opstack);
    push(s.opstack, boolector_sgt(s.runner->btor, x, y));
    s.release(x);
    s.release(y);
  }
  void Le::execute(MachineState &s) {
    BtorNode* y = pop(s.opstack);
    BtorNode* x = pop(s.opstack);
    push(s.opstack, boolector_slte(s.runner->btor, x, y));
    s.release(x);
    s.release(y);
  }
  void Ge::execute(MachineState &s) {
    BtorNode* y = pop(s.opstack);
    BtorNode* x = pop(s.opstack);
    push(s.opstack, boolector_sgte(s.runner->btor, x, y));
    s.release(x);
    s.release(y);
  }
  void Ne::execute(MachineState &s) {
    BtorNode* y = pop(s.opstack);
    BtorNode* x = pop(s.opstack);
    push(s.opstack, boolector_ne(s.runner->btor, x, y));
    s.release(x);
    s.release(y);
  }
  void LoadConst::execute(MachineState &s) {
    push(s.opstack, boolector_int(s.runner->btor, value, BV_WIDTH));
  }
  void LoadVar::execute(MachineState &s) {
    push(s.opstack, s.copy(s.env().at(name)));
  }
  void Pop::execute(MachineState &s) {
    s.release(pop(s.opstack));
  }
  void LoadSymbolic::execute(MachineState &s) {
    push(s.opstack, boolector_var(s.runner->btor, BV_WIDTH, NULL));
  }
  void Check::execute(MachineState &s) {
    BtorNode* &e = s.opstack.back();
    s.apply_assumptions();
    BtorNode* not_e = boolector_not(s.runner->btor, e);
    boolector_assume(s.runner->btor, not_e);
    int stat = boolector_sat(s.runner->btor);
    s.release(not_e);
    switch (stat) {
    case BOOLECTOR_UNSAT:
      break;
    case BOOLECTOR_SAT:
      {
	std::cout << "Assertion can fail!\nHere's how:\n";
	auto tt = s.trace.rbegin();
	for (auto it = s.envstack.rbegin(), ie = s.envstack.rend();
	     it != ie; ++it, ++tt)
	  {
	    std::cout << "In " << *tt << ":\n";
	    for (auto vit = it->begin(), vie = it->end();
		 vit != vie; ++vit)
	      {
		char *sval = boolector_bv_assignment(s.runner->btor, vit->second);
		std::string ssval(sval);
		std::replace(ssval.begin(), ssval.end(), 'x', '0');
		int ival = std::stoi(ssval, 0, 2);
		if (ival & (1 << (BV_WIDTH-1))) {
		  ival = -((1<<BV_WIDTH)-ival);
		}
		std::cout << "  " << vit->first << " = " << sval << " (" << ival << ")\n";
		boolector_free_bv_assignment(s.runner->btor, sval);
	      }
	  }
      }
      break;
    }
  }
  void Assume::execute(MachineState &s) {
    s.assumptions.push_back(s.copy(s.opstack.back()));
    s.apply_assumptions();
    if (boolector_sat(s.runner->btor) == BOOLECTOR_UNSAT) {
      throw assumptions_failed();
    }
  }
}

static std::unordered_set<std::string> builtins =
  { "assert", "assume", "symbolic", "check" };

void MachineState::apply_assumptions() {
  std::for_each(assumptions.begin(), assumptions.end(),
		[this](BtorNode* e){boolector_assume(runner->btor, e);});
}

Status MachineState::checkStatus(BtorNode* e)
{
  apply_assumptions();
  boolector_assume(runner->btor, e);
  int pos = boolector_sat(runner->btor);
  apply_assumptions();
  BtorNode *not_e = boolector_not(runner->btor, e);
  boolector_assume(runner->btor, not_e);
  boolector_release(runner->btor, not_e);
  int neg = boolector_sat(runner->btor);
  if (pos == BOOLECTOR_SAT && neg == BOOLECTOR_SAT) {
    return TRUE_OR_FALSE;
  } else if (pos == BOOLECTOR_SAT && neg == BOOLECTOR_UNSAT) {
    return MUST_BE_TRUE;
  } else if (pos == BOOLECTOR_UNSAT && neg == BOOLECTOR_SAT) {
    return MUST_BE_FALSE;
  } else if (pos == BOOLECTOR_UNSAT && neg == BOOLECTOR_UNSAT) {
    return NOT_TRUE_NOR_FALSE;
  } else {
    return UNKNOWN;
  }
}

void Runner::compileBody(std::vector<Expr*> &body, std::vector<std::unique_ptr<Instruction> > &code)
{
  for (auto it = body.begin(), ie = body.end();
       it != ie; ++it)
    {
      compileExpr(*it, code);
      if (it != ie - 1) {
	code.emplace_back(new Instructions::Pop());
      }
    }
}

void Runner::compileExpr(Expr *e, std::vector<std::unique_ptr<Instruction> > &code)
{
  if (AssignExpr* ae = dynamic_cast<AssignExpr*>(e)) {
    compileExpr(ae->expr, code);
    code.emplace_back(new Instructions::Store(*ae->name));
  }
  else if (IfThenElseExpr *itee = dynamic_cast<IfThenElseExpr*>(e)) {
    compileExpr(itee->cond, code);
    Instructions::IfThenElse *i = new Instructions::IfThenElse();
    compileBody(*itee->iftrue, i->iftrue);
    compileBody(*itee->iffalse, i->iffalse);
    code.emplace_back(i);
  }
  else if (CallExpr *ce = dynamic_cast<CallExpr*>(e)) {
    for (auto it = ce->args->begin(), ie = ce->args->end();
	 it != ie; ++it)
      {
	compileExpr(*it, code);
      }
    if (*ce->target == "symbolic") {
      if (ce->args->size() != 0) {
	throw std::runtime_error("wrong number of arguments to builtin 'symbolic'");
      }
      code.emplace_back(new Instructions::LoadSymbolic());
    } else if (*ce->target == "check") {
      if (ce->args->size() != 1) {
	throw std::runtime_error("wrong number of arguments to builtin 'check'");
      }
      code.emplace_back(new Instructions::Check());
    } else if (*ce->target == "assert") {
      if (ce->args->size() != 1) {
	throw std::runtime_error("wrong number of arguments to builtin 'assert'");
      }
      code.emplace_back(new Instructions::Check());
      code.emplace_back(new Instructions::Assume());
    } else if (*ce->target == "assume") {
      if (ce->args->size() != 1) {
	throw std::runtime_error("wrong number of arguments to builtin 'assume'");
      }
      code.emplace_back(new Instructions::Assume());
    } else {
      code.emplace_back(new Instructions::Call(*ce->target));
    }
  }
  else if (BasicExpr *be = dynamic_cast<BasicExpr*>(e)) {
    std::for_each(be->children->begin(), be->children->end(),
		  [this,&code](Expr *se) { compileExpr(se, code); });
    switch (be->op) {
    case BasicExpr::Plus:
      code.emplace_back(new Instructions::Add());
      break;
    case BasicExpr::Minus:
      code.emplace_back(new Instructions::Sub());
      break;
    case BasicExpr::Times:
      code.emplace_back(new Instructions::Mult());
      break;
    case BasicExpr::Divide:
      code.emplace_back(new Instructions::Div());
      break;
    case BasicExpr::Mod:
      code.emplace_back(new Instructions::Mod());
      break;
    case BasicExpr::Eq:
      code.emplace_back(new Instructions::Eq());
      break;
    case BasicExpr::Lt:
      code.emplace_back(new Instructions::Lt());
      break;
    case BasicExpr::Gt:
      code.emplace_back(new Instructions::Gt());
      break;
    case BasicExpr::Le:
      code.emplace_back(new Instructions::Le());
      break;
    case BasicExpr::Ge:
      code.emplace_back(new Instructions::Ge());
      break;
    case BasicExpr::Ne:
      code.emplace_back(new Instructions::Ne());
      break;
    }
  }
  else if (ConstExpr *ce = dynamic_cast<ConstExpr*>(e)) {
    code.emplace_back(new Instructions::LoadConst(std::stol(*ce->cv)));
  }
  else if (VarExpr *ve = dynamic_cast<VarExpr*>(e)) {
    code.emplace_back(new Instructions::LoadVar(*ve->var));
  }
  else if (LabeledExpr *le = dynamic_cast<LabeledExpr*>(le)) {
    compileExpr(le->expr, code);
  }
  else {
    throw std::logic_error("unhandled expression during compilation");
  }
}

void Runner::compile(Program *p)
{
  for (auto it = p->functions.begin(), ie = p->functions.end();
       it != ie; ++it)
    {
      CompiledFunction *cf = new CompiledFunction();
      Function &f = **it;
      cf->name = *f.name;
      std::transform(f.params->begin(), f.params->end(), std::back_inserter(cf->params),
		     [] (std::string *sp) { return *sp; });
      compfunc_byname.insert(std::make_pair(cf->name, std::unique_ptr<CompiledFunction>(cf)));
      compileBody(*f.body, cf->code);
      cf->code.emplace_back(new Instructions::Return());
      std::cout << "compiled code for " << cf->name << ":\n";
      for (auto &i : cf->code) {
	for (auto &l : i->source()) {
	  std::cout << l << "\n";
	}
      }
    }
  for (auto it = p->suites.begin(), ie = p->suites.end();
       it != ie; ++it)
    {
      suites.emplace_back();
      CompiledSuite &cs = suites.back();
      Suite &s = **it;
      cs.name = *s.name;
      compileBody(*s.setup, cs.setup_code);
      for (auto ti = s.tests->begin(), te = s.tests->end();
	   ti != te; ++ti)
	{
	  cs.tests.emplace_back();
	  CompiledTest &ct = cs.tests.back();
	  Test &t = **ti;
	  ct.name = *t.name;
	  compileBody(*t.body, ct.code);
	}
    }
}

MachineState &Runner::fork(MachineState &s)
{
  active_states.emplace_back(s);
  return active_states.back();
}

void Runner::runSuites()
{
  for (auto it = suites.begin(), ie = suites.end();
       it != ie; ++it)
    {
      std::cout << "Running suite " << it->name << "\n";
      for (auto tt = it->tests.begin(), te = it->tests.end();
	   tt != te; ++tt)
	{
	  std::cout << " Running test " << tt->name << "\n";
	  active_states.emplace_back();
	  MachineState &s = active_states.back();
	  s.runner = this;
	  s.envstack.emplace_back();
	  s.trace.emplace_back(it->name + "/" + tt->name);
	  for (auto iit = it->setup_code.begin(), iie = it->setup_code.end();
	       iit != iie; ++iit)
	    {
	      s.code.push_back(iit->get());
	    }
	  for (auto iit = tt->code.begin(), iie = tt->code.end();
	       iit != iie; ++iit)
	    {
	      s.code.push_back(iit->get());
	    }
	  runStates();
	}
    }
}

void Runner::runStates()
{
  int completed = 0;
  while (active_states.size()) {
    MachineState &s = active_states.front();
    while (s.code.size()) {
      Instruction *i = s.code.front();
      s.code.pop_front();
      try {
	i->execute(s);
      } catch (assumptions_failed &af) {
	break;
      }
    }
    active_states.pop_front();
    ++completed;
    std::cout << "   Completed " << completed << " states, " << active_states.size() << " remain\n";
  }
}

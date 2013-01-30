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

template <typename F, typename T>
void splice(T &to, F &from) {
  to.insert(to.begin(), from.begin(), from.end());
}

namespace Instructions {
  void Store::execute(MachineState &s) {
    s.env()[dest] = s.opstack.back();
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
	s.assumptions.push_back(cond);
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
  }
  void Call::execute(MachineState &s) {
    CompiledFunction *target_fun = s.runner->compfunc_byname.at(target);
    s.envstack.emplace_back();
    for (auto it = target_fun->params.rbegin(), ie = target_fun->params.rend();
	 it != ie; ++it) {
      s.env()[*it] = pop(s.opstack);
    }
    s.trace.push_back(target_fun->name);
    splice(s.code, target_fun->code);
  }
  void Return::execute(MachineState &s) {
    s.envstack.pop_back();
    s.trace.pop_back();
  }
  void Add::execute(MachineState &s) {
    BtorNode* y = pop(s.opstack);
    BtorNode* x = pop(s.opstack);
    push(s.opstack, boolector_add(s.runner->btor, x, y));
  }
  void Sub::execute(MachineState &s) {
    BtorNode* y = pop(s.opstack);
    BtorNode* x = pop(s.opstack);
    push(s.opstack, boolector_sub(s.runner->btor, x, y));
  }
  void Mult::execute(MachineState &s) {
    BtorNode* y = pop(s.opstack);
    BtorNode* x = pop(s.opstack);
    push(s.opstack, boolector_mul(s.runner->btor, x, y));
  }
  void Div::execute(MachineState &s) {
    BtorNode* y = pop(s.opstack);
    BtorNode* x = pop(s.opstack);
    push(s.opstack, boolector_sdiv(s.runner->btor, x, y));
  }
  void Mod::execute(MachineState &s) {
    BtorNode* y = pop(s.opstack);
    BtorNode* x = pop(s.opstack);
    push(s.opstack, boolector_smod(s.runner->btor, x, y));
  }
  void Eq::execute(MachineState &s) {
    BtorNode* y = pop(s.opstack);
    BtorNode* x = pop(s.opstack);
    push(s.opstack, boolector_eq(s.runner->btor, x, y));
  }
  void Lt::execute(MachineState &s) {
    BtorNode* y = pop(s.opstack);
    BtorNode* x = pop(s.opstack);
    push(s.opstack, boolector_slt(s.runner->btor, x, y));
  }
  void Gt::execute(MachineState &s) {
    BtorNode* y = pop(s.opstack);
    BtorNode* x = pop(s.opstack);
    push(s.opstack, boolector_sgt(s.runner->btor, x, y));
  }
  void Le::execute(MachineState &s) {
    BtorNode* y = pop(s.opstack);
    BtorNode* x = pop(s.opstack);
    push(s.opstack, boolector_slte(s.runner->btor, x, y));
  }
  void Ge::execute(MachineState &s) {
    BtorNode* y = pop(s.opstack);
    BtorNode* x = pop(s.opstack);
    push(s.opstack, boolector_sgte(s.runner->btor, y, x));
  }
  void Ne::execute(MachineState &s) {
    BtorNode* y = pop(s.opstack);
    BtorNode* x = pop(s.opstack);
    push(s.opstack, boolector_ne(s.runner->btor, x, y));
  }
  void LoadConst::execute(MachineState &s) {
    push(s.opstack, boolector_int(s.runner->btor, value, BV_WIDTH));
  }
  void LoadVar::execute(MachineState &s) {
    push(s.opstack, s.env().at(name));
  }
  void Pop::execute(MachineState &s) {
    pop(s.opstack);
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
    s.assumptions.push_back(s.opstack.back());
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

void Runner::compileBody(std::vector<Expr*> &body, std::vector<Instruction*> &code)
{
  for (auto it = body.begin(), ie = body.end();
       it != ie; ++it)
    {
      compileExpr(*it, code);
      if (it != ie - 1) {
	code.push_back(new Instructions::Pop());
      }
    }
}

void Runner::compileExpr(Expr *e, std::vector<Instruction*> &code)
{
  if (AssignExpr* ae = dynamic_cast<AssignExpr*>(e)) {
    compileExpr(ae->expr, code);
    code.push_back(new Instructions::Store(*ae->name));
  }
  else if (IfThenElseExpr *itee = dynamic_cast<IfThenElseExpr*>(e)) {
    std::vector<Instruction*> iftrue, iffalse;
    compileBody(*itee->iftrue, iftrue);
    compileBody(*itee->iffalse, iffalse);
    compileExpr(itee->cond, code);
    code.push_back(new Instructions::IfThenElse(iftrue, iffalse));
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
      code.push_back(new Instructions::LoadSymbolic());
    } else if (*ce->target == "check") {
      if (ce->args->size() != 1) {
	throw std::runtime_error("wrong number of arguments to builtin 'check'");
      }
      code.push_back(new Instructions::Check());
    } else if (*ce->target == "assert") {
      if (ce->args->size() != 1) {
	throw std::runtime_error("wrong number of arguments to builtin 'assert'");
      }
      code.push_back(new Instructions::Check());
      code.push_back(new Instructions::Assume());
    } else if (*ce->target == "assume") {
      if (ce->args->size() != 1) {
	throw std::runtime_error("wrong number of arguments to builtin 'assume'");
      }
      code.push_back(new Instructions::Assume());
    } else {
      code.push_back(new Instructions::Call(*ce->target));
    }
  }
  else if (BasicExpr *be = dynamic_cast<BasicExpr*>(e)) {
    std::for_each(be->children->begin(), be->children->end(),
		  [this,&code](Expr *se) { compileExpr(se, code); });
    switch (be->op) {
    case BasicExpr::Plus:
      code.push_back(new Instructions::Add());
      break;
    case BasicExpr::Minus:
      code.push_back(new Instructions::Sub());
      break;
    case BasicExpr::Times:
      code.push_back(new Instructions::Mult());
      break;
    case BasicExpr::Divide:
      code.push_back(new Instructions::Div());
      break;
    case BasicExpr::Mod:
      code.push_back(new Instructions::Mod());
      break;
    case BasicExpr::Eq:
      code.push_back(new Instructions::Eq());
      break;
    case BasicExpr::Lt:
      code.push_back(new Instructions::Lt());
      break;
    case BasicExpr::Gt:
      code.push_back(new Instructions::Gt());
      break;
    case BasicExpr::Le:
      code.push_back(new Instructions::Le());
      break;
    case BasicExpr::Ge:
      code.push_back(new Instructions::Ge());
      break;
    case BasicExpr::Ne:
      code.push_back(new Instructions::Ne());
      break;
    }
  }
  else if (ConstExpr *ce = dynamic_cast<ConstExpr*>(e)) {
    code.push_back(new Instructions::LoadConst(std::stol(*ce->cv)));
  }
  else if (VarExpr *ve = dynamic_cast<VarExpr*>(e)) {
    code.push_back(new Instructions::LoadVar(*ve->var));
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
      compfunc_byname[cf->name] = cf;
      compileBody(*f.body, cf->code);
      cf->code.push_back(new Instructions::Return());
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
	  s.code.insert(s.code.end(), it->setup_code.begin(), it->setup_code.end());
	  s.code.insert(s.code.end(), tt->code.begin(), tt->code.end());
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
    std::cout << "Completed " << completed << " states, " << active_states.size() << " remain\n";
  }
}

#include "eval.hpp"

#include <stdexcept>

#define BV_WIDTH 8

static std::unordered_set<std::string> builtins =
  { "assert", "assume", "symbolic" };

Status Runner::checkStatus(CVC3::Expr e)
{
  CVC3::QueryResult pos = solver->checkUnsat(e);
  if (pos != CVC3::VALID) {
    solver->returnFromCheck();
  }
  CVC3::QueryResult neg = solver->checkUnsat(e.negate());
  if (neg != CVC3::VALID) {
    solver->returnFromCheck();
  }
  if (pos == CVC3::SATISFIABLE && neg == CVC3::SATISFIABLE) {
    return TRUE_OR_FALSE;
  } else if (pos == CVC3::SATISFIABLE && neg == CVC3::UNSATISFIABLE) {
    return MUST_BE_TRUE;
  } else if (pos == CVC3::UNSATISFIABLE && neg == CVC3::SATISFIABLE) {
    return MUST_BE_FALSE;
  } else if (pos == CVC3::UNSATISFIABLE && neg == CVC3::UNSATISFIABLE) {
    return NOT_TRUE_NOR_FALSE;
  } else {
    return UNKNOWN;
  }
}

CVC3::Expr State::evaluate(std::vector<Expr*> &exprs)
{
  CVC3::Expr ans;
  for (auto it = exprs.begin(), ie = exprs.end();
       it != ie; ++it)
    {
      ans = evaluate(*it);
    }
  return ans;
}

CVC3::Expr State::fun_symbolic()
{
  static int id = 0;
  return owner->solver->varExpr("var_" + std::to_string(++id), owner->solver->bitvecType(BV_WIDTH));
}

CVC3::Expr State::fun_assume(CVC3::Expr e)
{
  owner->solver->assertFormula(e);
  return e;
}

CVC3::Expr State::fun_assert(CVC3::Expr e)
{
  CVC3::QueryResult stat = owner->solver->query(e);
  std::vector<std::string> reasons;
  if (owner->solver->incomplete(reasons)) {
    std::cout << "Incomplete results, reasons follow.\n";
    for (auto it = reasons.begin(), ie = reasons.end();
	 it != ie; ++it)
      {
	std::cout << *it << "\n";
      }
  }
  switch (stat) {
  case CVC3::VALID:
    break;
  case CVC3::INVALID:
    {
      CVC3::ExprMap<CVC3::Expr> ce;
      owner->solver->getConcreteModel(ce);
      std::cout << "Assertion can fail!\nHere's how:\n";
      for (auto it = callstack.rbegin(), ie = callstack.rend();
	   it != ie; ++it)
	{
	  std::cout << "In " << it->name << ":\n";
	  for (auto vit = it->env.begin(), vie = it->env.end();
	       vit != vie; ++vit)
	    {
	      std::cout << "  " << vit->first << " = " << owner->solver->getValue(vit->second) << "\n";
	    }
	}
    }
    break;
  case CVC3::ABORT:
    throw std::runtime_error("unhandled ABORT");
    break;
  case CVC3::UNKNOWN:
    throw std::runtime_error("unhandled UNKNOWN");
    break;
  }
  return e;
}

CVC3::Expr State::evaluate(Expr *e)
{
  std::unordered_map<std::string, CVC3::Expr> &env = callstack.back().env;
  if (AssignExpr *ae = dynamic_cast<AssignExpr*>(e)) {
    CVC3::Expr result = evaluate(ae->expr);
    env.insert(std::make_pair(*ae->name, result));
    return result;
  }
  else if (IfThenElseExpr *ite = dynamic_cast<IfThenElseExpr*>(e)) {
    CVC3::Expr cond_result = evaluate(ite->cond);
    switch (owner->checkStatus(cond_result)) {
    case MUST_BE_TRUE:
      return evaluate(*ite->iftrue);
    case MUST_BE_FALSE:
      return evaluate(*ite->iffalse);
    case TRUE_OR_FALSE:
      {
	int stacklevel = owner->solver->stackLevel();
	State truebranch(*this), falsebranch(*this);
	owner->solver->assertFormula(cond_result);
	CVC3::Expr true_ans = truebranch.evaluate(ite->iftrue);
	owner->solver->popto(stacklevel);
	owner->solver->assertFormula(cond_result.negate());
	CVC3::Expr false_ans = falsebranch.evaluate(ite->iffalse);
	owner->solver->popto(stacklevel);
	/* now merge the environments */
	env.clear();
	std::unordered_map<std::string, CVC3::Expr> &true_env = truebranch.curframe().env;
	std::unordered_map<std::string, CVC3::Expr> &false_env = falsebranch.curframe().env;
	for (auto it = true_env.begin(), ie = true_env.end();
	     it != ie; ++it)
	  {
	    if (false_env.count(it->first)) {
	      env.insert(std::make_pair(it->first,
					owner->solver->iteExpr(cond_result,
							      it->second,
							      false_env[it->first])));
	    }
	  }
	return owner->solver->iteExpr(cond_result, true_ans, false_ans);
      }
    case NOT_TRUE_NOR_FALSE:
      /* the state is invalid */
      throw std::runtime_error("Invalid state reached!");
    case UNKNOWN:
      throw std::runtime_error("Condition is unknown!");
    }
  }
  else if (CallExpr *ce = dynamic_cast<CallExpr*>(e)) {
    std::string &target_name = *ce->target;
    if (target_name == "symbolic") {
      return fun_symbolic();
    }
    else if (target_name == "assume") {
      if (ce->args->size() != 1) {
	throw std::runtime_error("Wrong number of arguments to builtin function assume, expected 1 got " + std::to_string(ce->args->size()));
      }
      return fun_assume(evaluate(ce->args->at(0)));
    }
    else if (target_name == "assert") {
      if (ce->args->size() != 1) {
	throw std::runtime_error("Wrong number of arguments to builtin function assume, expected 1 got " + std::to_string(ce->args->size()));
      }
      return fun_assert(evaluate(ce->args->at(0)));
    }
    Function *fun = owner->func_byname[*ce->target];
    if (fun == NULL) {
      throw std::runtime_error("Undefined function " + *ce->target);
    }
    if (fun->params->size() != ce->args->size()) {
      throw std::runtime_error("Wrong number of arguments to " + *fun->name + ", expected " + std::to_string(fun->params->size()) + " got " + std::to_string(ce->args->size()));
    }
    std::vector<CVC3::Expr> arg_vals;
    arg_vals.reserve(ce->args->size());
    for (auto it = ce->args->begin(), ie = ce->args->end();
	 it != ie; ++it)
      {
	arg_vals.push_back(evaluate(*it));
      }
    callstack.emplace_back(this, target_name);
    auto ai = arg_vals.begin();
    for (auto pi = fun->params->begin(), pe = fun->params->end();
	 pi != pe; ++pi, ++ai)
      {
	callstack.back().env.insert(std::make_pair(**pi, *ai));
      }
    CVC3::Expr result = evaluate(fun->body);
    callstack.pop_back();
    return result;
  }
  else if (BasicExpr *be = dynamic_cast<BasicExpr*>(e)) {
    std::vector<CVC3::Expr> child_vals;
    child_vals.reserve(be->children->size());
    for (auto it = be->children->begin(), ie = be->children->end();
	 it != ie; ++it)
      {
	child_vals.push_back(evaluate(*it));
      }
    switch (be->op) {
    case BasicExpr::Plus:
      return owner->solver->newBVPlusExpr(BV_WIDTH, child_vals[0], child_vals[1]);
    case BasicExpr::Minus:
      return owner->solver->newBVSubExpr(child_vals[0], child_vals[1]);
    case BasicExpr::Times:
      return owner->solver->newBVMultExpr(BV_WIDTH, child_vals[0], child_vals[1]);
    case BasicExpr::Divide:
      return owner->solver->newBVSDivExpr(child_vals[0], child_vals[1]);
    case BasicExpr::Mod:
      return owner->solver->newBVSModExpr(child_vals[0], child_vals[1]);
    case BasicExpr::Eq:
      return owner->solver->eqExpr(child_vals[0], child_vals[1]);
    case BasicExpr::Lt:
      return owner->solver->newBVSLTExpr(child_vals[0], child_vals[1]);
    case BasicExpr::Gt:
      return owner->solver->newBVSLTExpr(child_vals[1], child_vals[0]);
    case BasicExpr::Le:
      return owner->solver->newBVSLEExpr(child_vals[0], child_vals[1]);
    case BasicExpr::Ge:
      return owner->solver->newBVSLEExpr(child_vals[1], child_vals[0]);
    case BasicExpr::Ne:
      return owner->solver->eqExpr(child_vals[0], child_vals[1]).negate();
    }
  }
  else if (ConstExpr *ce = dynamic_cast<ConstExpr*>(e)) {
    return owner->solver->newBVConstExpr(CVC3::Rational(std::stol(*ce->cv)), BV_WIDTH);
  }
  else if (VarExpr *ve = dynamic_cast<VarExpr*>(e)) {
    auto it = env.find(*ve->var);
    if (it == env.end()) {
      throw std::runtime_error("Reference to undefined variable '" + *ve->var + "'");
    } else {
      return it->second;
    }
  }
  else if (LabeledExpr *le = dynamic_cast<LabeledExpr*>(e)) {
    return evaluate(le->expr);
  }
  else {
    throw std::logic_error(std::string("expression type ") + typeid(e).name() + " not handled");
  }
  throw std::logic_error("some case forgot to return");
}

void Runner::run(Program *p)
{
  for (auto it = p->functions.begin(), ie = p->functions.end();
       it != ie; ++it)
    {
      Function &f = **it;
      if (builtins.count(*f.name)) {
	std::cerr << "Cannot replace builtin function " << *f.name << "\n";
	return;
      }
      if (func_byname.count(*f.name)) {
	std::cerr << "Function " << *f.name << " already defined\n";
	return;
      }
      func_byname.insert(std::make_pair(*f.name, &f));
    }
  for (auto it = p->suites.begin(), ie = p->suites.end();
       it != ie; ++it)
    {
      Suite &suite = **it;
      for (auto it = suite.tests->begin(), ie = suite.tests->end();
	   it != ie; ++it)
	{
	  Test &test = **it;
	  State state(this);
	  try {
	    state.evaluate(suite.setup);
	    state.evaluate(test.body);
	  } catch (CVC3::Exception &e) {
	    std::cout << "Exception from CVC3: " << e.toString() << "\n";
	  }
	}
    }
}

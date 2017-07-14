#ifndef _FOL_H
#define _FOL_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <functional>

using namespace std;

typedef string FunctionSymbol;
typedef string PredicateSymbol;


class BaseTerm;
typedef shared_ptr<BaseTerm> Term;


class BaseTerm : public enable_shared_from_this<BaseTerm> {

public:
  enum Type { TT_FUNCTION };
  virtual Type getType() const = 0;
  virtual void printTerm(ostream & ostr) const = 0;
  virtual bool equalTo(const Term &t) const = 0;
  virtual ~BaseTerm() {}
};


class FunctionTerm : public BaseTerm {
private:
  FunctionSymbol _f;
  vector<Term> _ops;

public:
  FunctionTerm(const FunctionSymbol & f, 
	       const vector<Term> & ops)
    :_f(f),
     _ops(ops)
  {}
  FunctionTerm(const FunctionSymbol & f, 
	       vector<Term> && ops = vector<Term> ())
    :_f(f),
     _ops(ops)
  {} 

  virtual Type getType() const
  {
    return TT_FUNCTION;
  }

  const FunctionSymbol & getSymbol() const
  {
    return _f;
  }

  const vector<Term> & getOperands() const
  {
    return _ops;
  }

  virtual void printTerm(ostream & ostr) const
  {
    ostr << _f;

    for(unsigned i = 0; i < _ops.size(); i++)
      {
	if(i == 0)
	  ostr << "(";
	_ops[i]->printTerm(ostr);
	if(i != _ops.size() - 1)
	  ostr << ",";
	else
	  ostr << ")";
      }
  }
   virtual bool equalTo(const Term &t) const{
	
	if(_f != ((FunctionTerm *) t.get())->getSymbol())
		return false;

	const vector<Term> & t_ops = ((FunctionTerm *) t.get())->getOperands();

	if(_ops.size() != t_ops.size())
		return false;

	for(unsigned i = 0; i < _ops.size(); i++)
	if(!_ops[i]->equalTo(t_ops[i]))
		return false;

	return true;
   }
};

class BaseFormula;
typedef shared_ptr<BaseFormula> Formula;

class BaseFormula : public enable_shared_from_this<BaseFormula> {
  
public:

  enum Type { T_ATOM };

  virtual void printFormula(ostream & ostr) const = 0;
  virtual Type getType() const = 0;
  virtual ~BaseFormula() {}
};



class AtomicFormula : public BaseFormula {
public:
};

class Atom : public AtomicFormula {
protected:
  PredicateSymbol _p;
  vector<Term> _ops;

public:
  Atom(const PredicateSymbol & p, 
       const vector<Term> & ops)
    :_p(p),
     _ops(ops)
  {}
  Atom(const PredicateSymbol & p, 
        vector<Term> && ops = vector<Term>())
    :_p(p),
     _ops(ops)
  {}

  const PredicateSymbol & getSymbol() const
  {
    return _p;
  }

  const vector<Term> & getOperands() const
  {
    return _ops;
  }

  virtual void printFormula(ostream & ostr) const
  {
    ostr << _p;
    for(unsigned i = 0; i < _ops.size(); i++)
      {
	if(i == 0)
	  ostr << "(";
	_ops[i]->printTerm(ostr);
	if(i != _ops.size() - 1)
	  ostr << ",";
	else
	  ostr << ")";
      }
  }

  virtual Type getType() const
  {
    return T_ATOM;
  }

};

class Equality : public Atom {
public:
  Equality(const Term & lop, const Term & rop)
    :Atom("=", vector<Term> ())
  {
    _ops.push_back(lop);
    _ops.push_back(rop);
  }
  
  const Term & getLeftOperand() const
  {
    return _ops[0];
  }
  
  const Term & getRightOperand() const
  {
    return _ops[1];
  }

  virtual void printFormula(ostream & ostr) const
  {
    _ops[0]->printTerm(ostr);
    ostr << " = ";
    _ops[1]->printTerm(ostr);
  }
};


inline
ostream & operator << (ostream & ostr, const Term & t)
{
   t->printTerm(ostr);
  return ostr;
}

inline
ostream & operator << (ostream & ostr, const Formula & f)
{
  f->printFormula(ostr);
  return ostr;
}
inline 
bool operator==(const Term& lhs, const Term& rhs){ 
	return lhs->equalTo(rhs);
}


extern Formula parsed_formula;
extern vector<Formula>* parsed_set_of_formulas;
#endif // _FOL_H

#include "fol.hpp"
#include <vector>
#include <set>
#include <map>
using namespace std;
extern int yyparse();

/* Ovaj pokazivac ce nakon parsiranja dobiti vrednost 
   adrese parsirane formule. */
extern Formula parsed_formula;
extern vector<Formula>* parsed_set_of_formulas;

/*bool comparator(const Term &lhs, const Term &rhs)
{
    return !lhs->equalTo(rhs);
}*/
struct C
{
    bool operator()(const Term &a, const Term &b) const
    {
        return !a->equalTo(b);
    }
};

//typedef set<Term, bool(*)(const Term&, const Term&)> TermSet;
typedef set<Term, C> TermSet;
typedef map<Term, TermSet> UseMap;



void getTermsFromTerm(FunctionTerm *ft, TermSet& allTerms){
	Term t = make_shared<FunctionTerm>(ft->getSymbol(),std::move(ft->getOperands()));
	allTerms.insert(t);
	if(ft->getOperands().size()==0){
		return ;
	}
	for(auto i:ft->getOperands())
	{
		FunctionTerm* temp = (FunctionTerm*)i.get();
		getTermsFromTerm(temp,allTerms);
	}
	
}
void getTermsFromFormula(Formula f,  TermSet& allTerms ){
	Equality *atom = (Equality*)f.get();
	FunctionTerm* ft1 = (FunctionTerm*)atom->getLeftOperand().get();
	getTermsFromTerm(ft1,allTerms);
	FunctionTerm* ft2 = (FunctionTerm*)atom->getRightOperand().get();
	getTermsFromTerm(ft2,allTerms);
}
void getTerms(vector<Formula>* vf, Formula f, TermSet& allTerms){
	for(auto i: *vf)
		getTermsFromFormula(i,allTerms);
	getTermsFromFormula(f,allTerms);
}
//{a=c} {c=r};
void getUseMap(UseMap &um, TermSet &t){
	//prolazimo kroz sve termove
	for(auto i: t){
		//TermSet temp(&comparator);
		TermSet temp;
		//prolazimo kroz termove da vidimo da li je term i deo nekog terma
		for(auto j: t){
			FunctionTerm* f = (FunctionTerm*)j.get();
			const vector<Term> &terms = f->getOperands();
			for(unsigned k=0;k<terms.size();k++){
				//ako naidjemo da je deo nekog terma, onda roditelja od tog terma ubacujemo u skup
				if(i->equalTo(terms[k]))
					temp.insert(j);
			}
		}
		um.insert(make_pair(i,temp));
	}
}
int main()
{
  yyparse();
  
  if(parsed_formula.get() != 0)
    cout << parsed_formula;
  cout<< "\na sad formule" << endl;
  
  
  for(auto i:*parsed_set_of_formulas){
	  cout<<i<<endl;
  }
  cout << "a sad skup termova"<<endl;
 // TermSet allTerms(&comparator);
  TermSet allTerms;
  getTerms(parsed_set_of_formulas, parsed_formula, allTerms);
  for(auto i:allTerms)
		cout << i <<" ";
  cout<<endl;
  cout << "a sad skup use" << endl;
  UseMap um;
  getUseMap(um,allTerms);
   for(auto i:um){
		cout << i.first <<" : ";
		for(auto v:i.second)
			cout << v << ", ";
		cout<<endl;
   }
  cout<<endl;
  return 0;
}
//TODO::PROVERI ZASTO SET NE RADI DOBRO, IZBACUJE NEKE ISTEEEEE!!!!
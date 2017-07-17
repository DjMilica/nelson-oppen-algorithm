#include "fol.hpp"
#include "unionFind.hpp"
#include <vector>
#include <set>
#include <map>
using namespace std;
extern int yyparse();

/* Ovaj pokazivac ce nakon parsiranja dobiti vrednost 
   adrese parsirane formule. */
extern Formula parsed_formula;
extern vector<Formula>* parsed_set_of_formulas;


void getTermsFromTerm(FunctionTerm *ft, TermSet& allTerms){ 
   Term t = make_shared<FunctionTerm>(ft->getSymbol(),ft->getOperands());
   /*bool exists = false;  //NOTE ovo je bilo dok nije radio operator <
   for(auto term: allTerms)
      if(term==t)
         exists = true;
   if(!exists) */
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

void getUseMap(UseMap &um, TermSet &t){
   //prolazimo kroz sve termove
   for(auto i: t){
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
void printUseMap(UseMap &um){
   for(auto i:um){
      cout << i.first <<" : ";
      for(auto v:i.second)
         cout << v << " ";
      cout<<endl;
   }
   cout<<endl;
}
bool cong(Term firstTerm, Term secondTerm, UnionFind &u){
   FunctionTerm* f = (FunctionTerm*)firstTerm.get();
   const vector<Term> &terms1 = f->getOperands();
   FunctionTerm* g = (FunctionTerm*)secondTerm.get();
   const vector<Term> &terms2 = g->getOperands();
   if(f->getSymbol()!=g->getSymbol())
      return false;
   for(unsigned i=0; i<terms1.size(); i++){
      if(u.findRootOfTerm(terms1[i]) != u.findRootOfTerm(terms2[i]))
         return false;
   }
   return true;
}

//ovo sad je funkcija koja izracunava P_s = U { use(s') | find(s') == find(s) }
TermSet getSetsForMerge(Term s, UnionFind &u){
   UseMap& um = u.getUMap();
   TermSet equivalencySet = u.findTermsFromTheSameSet(s);
   TermSet returnValue;

   TermSet use = um[s]; 
   /*TermSet use; NOTE ovo je bilo dok nije radio operator <
   for(auto pair: um)
      if(pair.first==s)
         use = pair.second;*/
   for(auto term: use){
      returnValue.insert(term);
   }
   for(auto t: equivalencySet){
      //sve koji su use od t ubaci u konacni skup
      TermSet use = um[t];
      for(auto term: use){
         returnValue.insert(term);
      }
   }
   return returnValue;
}
void merge(Term s, Term t, UnionFind &u){
   TermSet firstSet = getSetsForMerge(s,u);
   TermSet secondSet = getSetsForMerge(t,u);
   u.unionOfSets(s,t);
   for(auto a: firstSet){
      for(auto b: secondSet){
         if(u.findRootOfTerm(a)!=u.findRootOfTerm(b) && cong(a,b,u)){
            merge(a,b,u);
         }
      }
   }
}
void cc(vector<Formula>* E, TermSet T, UnionFind &u){
   for(auto f: *E){
      Equality *atom = (Equality*)f.get();
      const Term &s = atom->getLeftOperand();
      const Term &t = atom->getRightOperand();
      int root1 = u.findRootOfTerm(s);
      int root2 = u.findRootOfTerm(t);
      if(root1!=root2){
         merge(s,t,u);
      }
   }
}
//funkcija proverava da li se termovi iz jednakosti iz Formule f nalaze u okviru iste klase kongruencije
bool checkEquality(vector<Formula>* E, Formula f){
   TermSet allTerms;
   getTerms(E, f, allTerms);
   UseMap um;
   getUseMap(um,allTerms);
   UnionFind u(allTerms, um);
   cc(E, allTerms, u);
   
   //sad proveravanje da li termovi iz f pripadaju istoj klasi ekvivalencije
   Equality *atom = (Equality*)f.get();
   const Term& ft1 = atom->getLeftOperand();
   const Term& ft2 = atom->getRightOperand();
   cout<< "**************Ovo su klase ekvivalencije*******************" << endl;
   u.printUnionFind(); 
   cout << "**********************************************************" << endl;
   TermSet roots = u.findAllRoots();
   for(auto term: roots){
      TermSet classEqualency = u.findTermsFromTheSameSet(term);
      if(classEqualency.find(ft1)!=classEqualency.end() || term==ft1){
         if(classEqualency.find(ft2)!=classEqualency.end() || term==ft2){
            return true;
         }
      }
   }
   return false;
}
//ako posmatramo formulu iz druge {} kao nejednakost, proveravamo zadovoljivost
void checkSAT(vector<Formula>* E, Formula f){
   if(checkEquality(E,f))
      cout << "Formula nije zadovoljiva jer se termovi iz formule " << f << " nalaze u istoj klasi kongruencije!" << endl;
   else
      cout << "Formula je zadovoljiva jer se termovi iz formule " << f << " nalaze u razlicitim klasama kongruencije!" << endl;
   
}
//{f(a,b)=a,f(b,a)=b} {f(f(a,b),f(b,a))=a}
//{y=f(x),x=g(y)} {x=g(f(x))};
//{x=f(x)} {x=f(f(f(x)))};
//{y=f(x),x=f(y)} {y=f(f(x))};
//{f(x,y) = f(y,x),g(x,y) = f(f(x,y),f(y,x)),g(y,x) = f(f(y,x), f(x,y))} {g(x,y) = g(y,x)};
//{f(g(a)) = g(f(a)),g(g(a)) = f(a), f(f(a)) = g(a)} { g(f(g(g(a)))) = f(a)};
int main()
{
   yyparse();

   /*if(parsed_formula.get() != 0)
      cout << parsed_formula;
   cout << "a sad skup termova"<<endl;
   TermSet allTerms;
   getTerms(parsed_set_of_formulas, parsed_formula, allTerms);
   for(auto i:allTerms)
      cout << i <<" ";
   cout<<endl;
   cout << "          a sad skup use            " << endl;
   UseMap um;
   getUseMap(um,allTerms);
   for(auto i:um){
      cout << i.first <<" : ";
      for(auto v:i.second)
      cout << v << ", ";
      cout<<endl;
   }
   cout<<endl; */
   /*************rad sa union find*******************/
   /*cout<< "         Union find        " << endl;
   UnionFind u(allTerms, um);
   u.printUnionFind(); 
   cout << endl;
   cc(parsed_set_of_formulas, allTerms, u);
   u.printUnionFind();*/
   if(checkEquality(parsed_set_of_formulas,parsed_formula))
      cout << "Vazi" << endl;
   else
      cout << "Ne vazi" << endl;
   
   //checkSAT(parsed_set_of_formulas, parsed_formula);
   return 0;
}

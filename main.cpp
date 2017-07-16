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
//ovo sad je funkcija koja izracunava P_s = U { use(s') | find(s') == find(s) }
TermSet getSetsForMerge(Term s, UnionFind &u){
   UseMap& um = u.getUMap();
   TermSet equivalencySet = u.findTermsFromTheSameSet(s);
   TermSet returnValue;
   for(auto t: um[s])
      returnValue.insert(t);
   for(auto t: equivalencySet){
      //sve koji su use od t ubaci u konacni skup
      cout<< "MA NESTO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
      TermSet use = um[t];
      for(auto term: use){
         cout << "########### printujem term " << term << endl; 
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
         cout << "$$$$$$$$$$ Radim proveru za: " << a << " i " << b << endl;
         if(u.findRootOfTerm(a)!=u.findRootOfTerm(b) && u.cong(a,b)){
            cout<<"**********Merge iz merge za: " << a << " i " << b << "**************" << endl;
            merge(a,b,u);
            cout << "***************union find**: ";
            u.printUnionFind();
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
         cout<<"**********Merge iz cc za: " << s << " i " << t << "**************" << endl; 
         merge(s,t,u);
         cout << "***************union find**: ";
         u.printUnionFind();
      }
   }
}
//{f(a,b)=a,f(b,a)=b} {f(f(a,b),f(b,a))=a}
int main()
{
   yyparse();

   /*if(parsed_formula.get() != 0)
      cout << parsed_formula;
   cout<< "\na sad formule" << endl;
   for(auto i:*parsed_set_of_formulas){
      cout<<i<<endl;
   }
   cout << "a sad skup termova"<<endl;*/
   TermSet allTerms;
   getTerms(parsed_set_of_formulas, parsed_formula, allTerms);
   /*for(auto i:allTerms)
      cout << i <<" ";
   cout<<endl;*/
   cout << "          a sad skup use            " << endl;
   UseMap um;
   getUseMap(um,allTerms);
   for(auto i:um){
      cout << i.first <<" : ";
      for(auto v:i.second)
      cout << v << ", ";
      cout<<endl;
   }
   cout<<endl; 
   /*************rad sa union find*******************/
   cout<< "         Union find        " << endl;
   UnionFind u(allTerms, um);
   u.printUnionFind(); 
   cc(parsed_set_of_formulas, allTerms, u);
   u.printUnionFind();
   return 0;
}

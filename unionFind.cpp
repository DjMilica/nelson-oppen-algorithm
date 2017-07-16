#include "unionFind.hpp"
#include <set>
//kada pravimo union find strukturu, na pocetku je svaki element u razlicitim klasama ekvivalencije
UnionFind::UnionFind(TermSet &t, UseMap &u) : _map(u)
{
   int i=0;
   for(auto term: t)
   {
      Node *n = new Node();
      n->t = term; //TODO ispitaj da li je ovo kriticno ili mora make_shared!
      n->parent = n;
      n->rank = 0;
      n->position = i;
      _nodes.push_back(n);
      i++;
   }
   _numOfSets = t.size();
}
int UnionFind::findPosition(Term s) const{
   for(unsigned i = 0; i < _nodes.size(); i++)
      if(s->equalTo(_nodes[i]->t)){
         return i;
      }
   return -1;
}

int UnionFind::findRootOfTerm(Term t) const{
   Node *current;
   unsigned i = findPosition(t);
   
   current = _nodes[i];
   while(current->parent != current)
      current = current->parent;
   return current->position;
   
   //TODO mozes da vidis ovde da napravis da svaki pokazuje bas na root-a, a ne na nekog svog parenta
   //ako nismo pronasli taj term, vracamo -1
   return -1;
}

void UnionFind::unionOfSets(Term firstTerm, Term secondTerm){
   Node* firstSet = _nodes[findRootOfTerm(firstTerm)];
   Node* secondSet = _nodes[findRootOfTerm(secondTerm)];

   if(firstSet == secondSet)
      return;

   if(firstSet->rank > secondSet->rank)
      secondSet->parent = firstSet;
   else if(firstSet->rank < secondSet->rank)
      firstSet->parent = secondSet;
   else{
      secondSet->parent = firstSet;
      firstSet->rank++; 
   }
   _numOfSets--;
}
TermSet UnionFind::findAllRoots() const{
   TermSet roots;
   for(unsigned i = 0; i < _nodes.size(); i++){
      roots.insert(_nodes[i]->parent->t);
   }
   return roots;
}
set<Node*> UnionFind::findAllRootNodes() const{
   set<Node*> nodes;
   for(unsigned i = 0; i < _nodes.size(); i++){
      nodes.insert(_nodes[i]->parent);
   }
   return nodes;
}

TermSet UnionFind::findTermsFromTheSameSet(Term t) const{
   TermSet set;
   unsigned i = findPosition(t);
   int root1 = findRootOfTerm(t);
   for(unsigned j=0;j<_nodes.size();j++){
      int root2 = findRootOfTerm(_nodes[j]->t);
      if(j!=i && root1==root2) //ako su im isti rootovi, a ne parent, ubaci u set
         set.insert(_nodes[j]->t);
   }
   return set;
}

void UnionFind::printUnionFind() const{
   set<Node*> roots = findAllRootNodes();
   int i=0;
   for (auto root: roots){
      cout << "Klasa ekvivalencije broj " << i << ":" << root->t;
      TermSet equivalencySet = findTermsFromTheSameSet(root->t);
      for(auto term: equivalencySet){
         cout << ", " << term;
      }
      cout << endl;
      i++;
   }

}


bool UnionFind::cong(Term firstTerm, Term secondTerm) const{
   cout<< "!!!!!!!!!!!!!!!!Radim cong za: " << firstTerm << " i " << secondTerm << endl;
   FunctionTerm* f = (FunctionTerm*)firstTerm.get();
   const vector<Term> &terms1 = f->getOperands();
   FunctionTerm* g = (FunctionTerm*)firstTerm.get();
   const vector<Term> &terms2 = g->getOperands();
   if(f->getSymbol()!=g->getSymbol())
      return false;
   for(unsigned i=0; i<terms1.size(); i++){
      if(findRootOfTerm(terms1[i]) != findRootOfTerm(terms2[i]))
         return false;
   }
   return true;
}
UseMap& UnionFind::getUMap(){
   return _map;
}
UnionFind::~UnionFind(){
   
}
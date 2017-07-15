#include "unionFind.hpp"
#include <set>
//kada pravimo union find strukturu, na pocetku je svaki element u razlicitim klasama ekvivalencije
UnionFind::UnionFind(TermSet &t)
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

int UnionFind::findRootOfTerm(Term t) const{
   Node *current;
   for(unsigned i = 0; i < _nodes.size(); i++)
      if(t->equalTo(_nodes[i]->t)){
      current = _nodes[i];
      while(current->parent != current)
         current = current->parent;
      return current->position;
      }
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
void UnionFind::printUnionFind() const{
   set<Node*> roots = findAllRootNodes();
   int i=0;
   for (auto root: roots){
      cout << "Klasa ekvivalencije broj " << i << ":" << root->t;
      for(unsigned j=0;j<_nodes.size();j++){
            if(_nodes[j]->parent == root && _nodes[j]!=root)
               cout<< ", " << _nodes[j]->t;
      }
      cout << endl;
      i++;
   }

}
#include "unionFind.hpp"

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
void UnionFind::printUnionFind() const{
	TermSet roots = findAllRoots();
	for (int i = 0; i < _numOfSets; ++i){
		cout << "Klasa ekvivalencije broj " << i << ":";
		
	}
	
}
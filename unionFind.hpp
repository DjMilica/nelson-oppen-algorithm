#ifndef _UNIONFIND_H
#define _UNIONFIND_H

#include <iostream>
#include <string>
#include <vector>
#include "fol.hpp"

struct Node
{
  Term t; //vrednost cvora
  Node *parent; //roditelj cvora
  int rank;
	int position;
};

class UnionFind{
private:
	vector<Node*> _nodes;
	int _numOfSets;
public:
	UnionFind(TermSet &t);
	int findRootOfTerm(Term t) const;
	void unionOfSets(Term firstTerm, Term secondTerm);	
	void printUnionFind() const;
	TermSet findAllRoots() const;
};

#endif
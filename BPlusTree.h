#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <iostream>
#include <vector>
#include "Node.h"

using namespace std;

//class BPlusTee
class BPlusTree
{
private:
	Node* root;       //save the pointer of root
	int order;        //save order of tree
	int splitIndex;   //save the index when split
	int minIndex;     //save mini num of keys in index node

public:
	BPlusTree(int);   //construct of tree
	~BPlusTree();     //desctruct of tree

	void Insert(int, float);         //Insert pair
	float Search(int);               //Search
	vector<float> Search(int,int);   //Search in range
	void Delete(int);                //Delete pair                    
	void printTree();                //Print tree     
	void printValues();              //Print the linked list of values
};

#endif
